#include <Password.h>
Password pass = Password("1234");
Password pass_m = Password("0000");
int c = 0;
int alarm = 0;
//*************************************************************************
#include <Keypad.h>
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
byte rowPins[ROWS] = { 22, 24, 26, 28 };
byte colPins[COLS] = { 30, 32, 34, 36 };
Keypad koko = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
char clik;
//*******************************************************************************

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
unsigned long lcd_delay = 0;
//***************************************************************************
#include <Servo.h>
Servo servo1;
Servo servo2;
Servo servo3;
#define servo1_pin 3
#define servo2_pin 4
#define servo3_pin 5

#define led1_pin 6
#define led2_pin 7
#define led3_pin 8

#define btn1_pin 9
#define btn2_pin 10
#define btn3_pin 11
#define buzz 48

//**********************************************************************
#define sim Serial1

#include <Adafruit_Fingerprint.h>
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial2);
uint8_t id;
unsigned long page_delay = 0;

int page = 1;
int del = 100;

String number = "+96893261458";

#include <Wire.h>
#include "RTClib.h"
RTC_DS1307 rtc;
unsigned long sec_del = 0;

int main_h = 0;
int main_m = 0;
int main_s = 0;
int main_D = 0;
int main_M = 0;
int main_Y = 0;
////////////////////////////////
// when get pass or finger right
bool medicine_1_allowed_to_open = false;
bool medicine_2_allowed_to_open = false;
bool medicine_3_allowed_to_open = false;

// when time hit
bool medicine_1_time = false;
bool medicine_2_time = false;
bool medicine_3_time = false;

// medicine time
int main_h1 = 19;
int main_m1 = 26;
int main_s1 = 0;
int main_D1 = 26;
int main_M1 = 5;
int main_Y1 = 2024;

int main_h2 = 19;
int main_m2 = 13;
int main_s2 = 0;
int main_D2 = 1;
int main_M2 = 5;
int main_Y2 = 2024;

int main_h3 = 21;
int main_m3 = 43;
int main_s3 = 0;
int main_D3 = 14;
int main_M3 = 4;
int main_Y3 = 2024;

void setup() {
  Serial.begin(9600);
  finger.begin(57600);
  sim.begin(9600);
  rtc.begin();
  //rtc.adjust(DateTime(2024, 5, 26, 19, 1, 0));

  pinMode(btn1_pin, INPUT_PULLUP);
  pinMode(btn2_pin, INPUT_PULLUP);
  pinMode(btn3_pin, INPUT_PULLUP);

  pinMode(led1_pin, OUTPUT);
  pinMode(led2_pin, OUTPUT);
  pinMode(led3_pin, OUTPUT);

  servo1.attach(servo1_pin);
  servo2.attach(servo2_pin);
  servo3.attach(servo3_pin);
  servo1.write(0);
  servo2.write(7);
  servo3.write(0);

  pinMode(buzz, OUTPUT);

  lcd.init();
  lcd.backlight();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("     SMART       ");
  lcd.setCursor(0, 1);
  lcd.print("    PHARMACY     ");
  delay(2000);
  lcd.clear();
  //send_msg("hii", 1);
  page_delay = millis();
}

void loop() {
  get_time_24();
  check_medicine();
  buttons_clicked();
  //change page every 3 sec
  if (millis() - page_delay > 3000) {
    page++;
    if (page > 3) page = 1;
    page_delay = millis();
  }

  //update page every 1 sec
  if (millis() - lcd_delay > 1000) {
    lcd.clear();
    if (page == 1) {
      lcd.setCursor(0, 0);
      lcd.print("Time : ");
      lcd.print(main_h);
      lcd.print(":");
      lcd.print(main_m);
      lcd.print(":");
      lcd.print(main_s);
      lcd.setCursor(0, 1);
      lcd.print("Date :");
      lcd.print(main_D);
      lcd.print("/");
      lcd.print(main_M);
      lcd.print("/");
      lcd.print(main_Y);
    } else if (page == 2) {
      lcd.setCursor(0, 0);
      lcd.print("A-Enter Sensor");
      lcd.setCursor(0, 1);
      lcd.print("B-Enter Password");
    } else if (page == 3) {
      lcd.setCursor(0, 0);
      lcd.print("C-Clear Database");
      lcd.setCursor(0, 1);
      lcd.print("D-Add Print");
    }

    lcd_delay = millis();
  }


  // wait for any keypad clicked
  clik = koko.getKey();
  if (clik) {
    Serial.println(clik);
    digitalWrite(buzz, 1);
    delay(del);
    digitalWrite(buzz, 0);
    lcd.clear();
    if (clik == 'A') {
      enter_finger();
    } else if (clik == 'B') {
      pass_();
    } else if (clik == 'C') {
      if (pass_f()) clear_finger();
    } else if (clik == 'D') {
      if (pass_f()) add_finger();
    } else if (clik == '*') {
      //servo.write(90);
    }
    delay(100);
    lcd.clear();
  }

  ///
  digitalWrite(led1_pin, medicine_1_time);
  digitalWrite(led2_pin, medicine_2_time);
  digitalWrite(led3_pin, medicine_3_time);
}

void enter_finger() {
  //lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Your Finger PLS!");
  while (getFingerprintIDez() < 0) {
    clik = koko.getKey();
    if (clik) {
      digitalWrite(buzz, 1);
      delay(del);
      digitalWrite(buzz, 0);
      delay(del);
      if (clik == 'B') return;
    }
  }
  lcd.setCursor(0, 1);
  lcd.print("Print accepted!");
  delay(3000);
  allow_to_open();
}

void add_finger() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter ID : ");
  lcd.setCursor(0, 1);
  int k = 0;
  while (1) {
    clik = koko.getKey();
    if (clik) {
      digitalWrite(buzz, 1);
      delay(del);
      digitalWrite(buzz, 0);
      delay(del);
      if ((clik == '1' || clik == '2' || clik == '3' || clik == '4' || clik == '5' || clik == '6' || clik == '7' || clik == '8' || clik == '9') & k == 0) {
        k++;
        lcd.setCursor(7, 1);
        lcd.print(clik);
        id = clik - '0';
      }
      if (clik == 'A' & k > 0) break;
      if (clik == 'B') return;
      if (clik == 'C') add_finger();
    }
  }

  getFingerprintEnroll();
  delay(2000);
}

void clear_finger() {
zz:
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Select mode :   ");
  lcd.setCursor(0, 1);
  lcd.print("A-All_Data  C-ID");
  while (1) {
    clik = koko.getKey();
    if (clik) {
      digitalWrite(buzz, 1);
      delay(del);
      digitalWrite(buzz, 0);
      delay(del);
      if (clik == 'A') {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Confirm :       ");
        lcd.setCursor(0, 1);
        lcd.print("A-Yes       B-No");
        while (1) {
          clik = koko.getKey();
          if (clik) {
            digitalWrite(buzz, 1);
            delay(del);
            digitalWrite(buzz, 0);
            delay(del);
            if (clik == 'A') {
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Deleting all...");
              delay(2000);
              finger.emptyDatabase();
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Done!");
              delay(1000);
              return;
            }
            if (clik == 'B') goto zz;
          }
        }
      }
      if (clik == 'B') return;
      if (clik == 'C') {
z:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Enter ID :");
        lcd.setCursor(0, 1);
        int k = 0;
        while (1) {
          clik = koko.getKey();
          if (clik) {
            digitalWrite(buzz, 1);
            delay(del);
            digitalWrite(buzz, 0);
            delay(del);
            if ((clik == '1' || clik == '2' || clik == '3' || clik == '4' || clik == '5' || clik == '6' || clik == '7' || clik == '8' || clik == '9') & k == 0) {
              k++;
              lcd.setCursor(7, 1);
              lcd.print(clik);
              id = clik - '0';
            }
            if (clik == 'A' & k > 0) break;
            if (clik == 'B') goto zz;
            if (clik == 'C') goto z;
          }
        }
        delay(1000);
        deleteFingerprint(id);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Select mode :   ");
        lcd.setCursor(0, 1);
        lcd.print("A-All_Data  C-ID");
      }
    }
  }
}


void pass_() {
  while (c < 4) {
    //lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" Enter Password ");
    clik = koko.getKey();
    if (clik) {
      digitalWrite(buzz, 1);
      delay(del);
      digitalWrite(buzz, 0);
      delay(del);
      if (clik == '1' || clik == '2' || clik == '3' || clik == '4' || clik == '5' || clik == '6' || clik == '7' || clik == '8' || clik == '9' || clik == '0') {
        lcd.setCursor(c + 6, 1);
        lcd.print("*");
        pass.append(clik);
        c++;
        delay(100);
      }
      if (clik == 'B') return;
    }
  }
  delay(1000);
  if (pass.evaluate()) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Password Correct");
    delay(2000);
    pass.reset();
    c = 0;
    lcd.clear();
    alarm = 0;
    allow_to_open();
    return;
  } else {
    lcd.clear();
    delay(200);
    lcd.setCursor(0, 0);
    lcd.print(" Wrong Password ");
    alarm++;
    delay(2000);
    pass.reset();
    c = 0;
    lcd.clear();
    if (alarm > 2) {
      unsigned long m = millis();
      delay(50);
      while (millis() - m < 10000) {
        lcd.setCursor(0, 0);
        lcd.print("System Is Locked");
        lcd.setCursor(7, 1);
        lcd.print(10 - ((millis() - m) / 1000));
        lcd.print("  ");
      }
      return;
    }
    pass_();
  }
}

bool pass_f() {
  while (c < 4) {
    //lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" Enter Password ");
    clik = koko.getKey();
    if (clik) {
      digitalWrite(buzz, 1);
      delay(del);
      digitalWrite(buzz, 0);
      delay(del);
      if (clik == '1' || clik == '2' || clik == '3' || clik == '4' || clik == '5' || clik == '6' || clik == '7' || clik == '8' || clik == '9' || clik == '0') {
        lcd.setCursor(c + 6, 1);
        lcd.print("*");
        pass_m.append(clik);
        c++;
        delay(100);
      }
      if (clik == 'B') return 0;
    }
  }
  delay(1000);
  if (pass_m.evaluate()) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Password Correct");
    delay(2000);

    pass_m.reset();
    c = 0;
    lcd.clear();
    return 1;
  } else {
    lcd.clear();
    delay(200);
    lcd.setCursor(0, 0);
    lcd.print(" Wrong Password ");
    delay(2000);
    pass_m.reset();
    c = 0;
    lcd.clear();
    pass_f();
  }
}


uint8_t getFingerprintEnroll() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Put your finger:");
  lcd.setCursor(0, 1);
  //delay(200);
  int p = -1;  // -1 to make the while condition always true

  while (p != FINGERPRINT_OK) {  //FINGERPRINT_OK==0x00 :: the condition break when p=FINGERPRINT_OK
    p = finger.getImage();       // get image from sensor right now and it's return the result
    switch (p) {
      case FINGERPRINT_OK:  //0x00
        //Serial.println("Image taken");
        //lcd.clear();
        lcd.print("Image taken");
        break;
      case FINGERPRINT_NOFINGER:  //0x02
        //Serial.println(".");
        break;
      default:
        // Serial.println("Unknown error");
        lcd.print("Error");
        break;
    }
  }
  delay(500);
  //lcd.clear();
  //lcd.print("Remove finger");
  //delay(2000);
  lcd.clear();
  // OK success!

  p = finger.image2Tz(1);  // 1 to to convert and store :: 2 to convert and verify (double cheak)
  // if ok continuo else exit from the hole function and restart the void loop
  switch (p) {
    case FINGERPRINT_OK:
      //Serial.println("Image converted");
      lcd.print("Image converted");
      break;
    default:
      //Serial.println("Unknown error");
      lcd.print("Error");
      return p;
  }
  delay(1000);
  //Serial.println("Remove finger");
  lcd.clear();
  lcd.print("Remove finger");
  delay(2000);
  p = 0;
  // the while is wait the user to unput his finger from sensor

  delay(500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Verify Finger:");
  lcd.setCursor(0, 1);
  // Serial.print("ID "); Serial.println(id);
  p = -1;
  // double cheak the finger read and verfiy
  //Serial.println("Place same finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        //Serial.println("Image taken");
        lcd.setCursor(0, 1);
        lcd.print("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        // Serial.print(".");
        break;
      default:
        lcd.setCursor(0, 1);
        lcd.print("Error");
        break;
    }
  }
  delay(500);
  lcd.clear();
  //lcd.print("Remove finger");
  //delay(2000);
  // OK success!
  lcd.clear();
  p = finger.image2Tz(2);  // converted as verefy not to store
  switch (p) {
    case FINGERPRINT_OK:
      //Serial.println("Image converted");
      lcd.print("Image converted");
      break;
    default:
      lcd.setCursor(0, 0);
      lcd.print("Error");
      return p;
  }
  delay(1000);
  lcd.clear();
  // OK converted!
  // Serial.print("Creating model for #");  Serial.println(id);
  // to create model it must match the first fingerprint and the double cheak

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    //Serial.println("Prints matched!");
    //  lcd.clear();
    lcd.print("Prints matched!");
    //myservo.write(0);
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    // Serial.println("Fingerprints did not match");
    lcd.print("Not matched!");
    return p;
  } else {
    // Serial.println("Unknown error");

    lcd.print("Error");
    return p;
  }
  delay(1500);
  lcd.clear();
  //  Serial.print("ID "); Serial.println(id);
  //if the model was created now we want to stor it in the choosen id
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    //Serial.println("Stored!");
    lcd.print("Stored!");
  } else {
    lcd.print("error");
    // Serial.println("Unknown error");
    return p;
  }
  delay(1000);
}


int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) return -1;

  return finger.fingerID;
}


uint8_t deleteFingerprint(uint8_t id) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Deleting ID: ");
  lcd.print(id);
  delay(500);
  lcd.setCursor(0, 1);
  uint8_t p = -1;  //for if condition
  p = finger.deleteModel(id);
  if (p == FINGERPRINT_OK) {
    lcd.print("Deleted");

  } else {
    lcd.print("error");
    return p;
  }
  delay(2000);
}


void get_time_24() {

  if (millis() - sec_del > 500) {
    DateTime now = rtc.now();

    main_h = now.hour();
    main_m = now.minute();
    main_s = now.second();

    main_Y = now.year();
    main_M = now.month();
    main_D = now.day();

    sec_del = millis();
  }
}

void check_medicine() {
  if (main_Y == main_Y1 && main_M == main_M1 && main_D == main_D1 && main_h == main_h1 && main_m == main_m1 && medicine_1_time == false) {
    medicine_1_time = true;
    Serial.println("medicine_1_time = true");
    send_msg("it's time for medicine 1 , expire date : 30/9/2024", 1);
  }
  if (medicine_1_time == true && main_m > (main_m1 + 5)) {
    medicine_1_time = false;
    Serial.println("medicine_1_time = false");
  }
  //////
  if (main_Y == main_Y2 && main_M == main_M2 && main_D == main_D2 && main_h == main_h2 && main_m == main_m2 && medicine_2_time == false) {
    medicine_2_time = true;
    Serial.println("medicine_2_time = true");
    send_msg("it's time for medicine 2 , expire date : 25/10/2026", 2);
  }
  if (medicine_2_time == true && main_m > (main_m2 + 5)) {
    medicine_2_time = false;
    Serial.println("medicine_2_time = false");
  }
  ///////
  if (main_Y == main_Y3 && main_M == main_M3 && main_D == main_D3 && main_h == main_h3 && main_m == main_m3 && medicine_3_time == false) {
    medicine_3_time = true;
    Serial.println("medicine_3_time = true");
    send_msg("it's time for medicine 3 , expire date : 1/1/2025", 3);
  }
  if (medicine_3_time == true && main_m > (main_m3 + 5)) {
    medicine_3_time = false;
    Serial.println("medicine_3_time = false");
  }
}

void allow_to_open() {
  if (medicine_1_time == true && medicine_1_allowed_to_open == false) {
    medicine_1_allowed_to_open = true;
    Serial.println("medicine_1_allowed_to_open = true");
  }
  if (medicine_2_time == true && medicine_2_allowed_to_open == false) {
    medicine_2_allowed_to_open = true;
    Serial.println("medicine_2_allowed_to_open = true");
  }
  if (medicine_3_time == true && medicine_3_allowed_to_open == false) {
    medicine_3_allowed_to_open = true;
    Serial.println("medicine_3_allowed_to_open = true");
  }
}

void buttons_clicked() {
  if (digitalRead(btn1_pin) == LOW) {
    Serial.println("btn1 clicked");
    if (medicine_1_allowed_to_open) {
      servo1.write(90);
      medicine_1_allowed_to_open = false;
      medicine_1_time = false;
      Serial.println("door1 open");
    } else {
      servo1.write(0);
      Serial.println("door1 close");
    }
    delay(500);
  }

  if (digitalRead(btn2_pin) == LOW) {
    Serial.println("btn2 clicked");
    if (medicine_2_allowed_to_open) {
      servo2.write(90);
      medicine_2_allowed_to_open = false;
      medicine_2_time = false;
      Serial.println("door2 open");
    } else {
      servo2.write(7);
      Serial.println("door2 close");
    }
    delay(500);
  }

  if (digitalRead(btn3_pin) == LOW) {
    Serial.println("btn3 clicked");
    if (medicine_3_allowed_to_open) {
      servo3.write(90);
      medicine_3_allowed_to_open = false;
      medicine_3_time = false;
      Serial.println("door3 open");
    } else {
      servo3.write(0);
      Serial.println("door3 close");
    }
    delay(500);
  }
}


void send_msg(String msg, int mid_num) {
  digitalWrite(buzz, 1);
  sim.println("AT+CMGF=1");
  delay(1000);
  sim.println("AT+CMGS=\"" + number + "\"\r");
  delay(1000);
  sim.println(msg);
  delay(1000);
  sim.println((char)26);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" It is Time For  ");
  lcd.setCursor(0, 1);
  lcd.print("   medicine ");
  lcd.print(mid_num);
  digitalWrite(buzz, 0);
  delay(1000);
}
