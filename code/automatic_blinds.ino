#include <Chrono.h>
#include <IRremote.hpp>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Wire.h>
#include <DS3231.h>


#define DHT_PIN 2
#define LDR_LEFT_PIN A0
#define LDR_RIGHT_PIN A1
#define MOTOR_LEFT_A 3
#define MOTOR_LEFT_B 5
#define MOTOR_RIGHT_A 6
#define MOTOR_RIGHT_B 9
#define MOTOR_VERTICAL_A 10
#define MOTOR_VERTICAL_B 11
#define LIMIT_RIGHT 4
#define RELAY_PIN A2
#define LED_PIN 13
#define LIMIT_LEFT 7
#define LIMIT_VERTICAL 8
#define IR_PIN 12


#define DHTTYPE DHT22

#define TEMP_UPPER_LIMIT 32
#define TEMP_LOWER_LIMIT 21

#define LDR_RIGHT_LIMIT 70
#define LDR_LEFT_LIMIT 70

#define MOTOR_LEFT_CLOSE_DURATION 3000
#define MOTOR_RIGHT_CLOSE_DURATION 3000

#define MOTOR_VERTICAL_1_2 3000
#define MOTOR_VERTICAL_2_3 3000
#define MOTOR_VERTICAL_1_3 6000
#define MOTOR_VERTICAL_3_2 3000



bool isLeftOpen = true;
bool isRightOpen = true;
int verticalState = 0;  //0=open 1=middle 2=close
int lightState = 0;     //0=off 1=low light 2=mid light 3=bright light
bool isOnAutomatic = true;


DS3231 myRTC;
bool isAm, isH12, isCentury;
int myHour = 0;

decode_results results;
DHT_Unified dht(DHT_PIN, DHTTYPE);

Chrono leftChrono, rightChrono, verticalChrono, timeChrono, IRchrono, sensorChrono;
int testMode, modeLeft, modeRight, modeVertical;

long mytime;



void setup() {
  Serial.begin(9600);
  Serial.setTimeout(100);

  pinMode(MOTOR_LEFT_A, OUTPUT);
  pinMode(MOTOR_LEFT_B, OUTPUT);
  pinMode(MOTOR_RIGHT_A, OUTPUT);
  pinMode(MOTOR_RIGHT_B, OUTPUT);
  pinMode(MOTOR_VERTICAL_A, OUTPUT);
  pinMode(MOTOR_VERTICAL_B, OUTPUT);
  pinMode(LIMIT_RIGHT, INPUT_PULLUP);
  pinMode(LIMIT_LEFT, INPUT_PULLUP);
  pinMode(LIMIT_VERTICAL, INPUT_PULLUP);
  pinMode(LDR_LEFT_PIN, INPUT);
  pinMode(LDR_RIGHT_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  allOff();
  dht.begin();
  IrReceiver.begin(12, false);
  Wire.begin();
  delay(500);

  // myRTC.setYear(24);
  // myRTC.setMonth(7);
  // myRTC.setDate(10);
  // myRTC.setHour(18);
  // myRTC.setMinute(57);
  // myRTC.setSecond(0);

  while (!initialize()) {
  }
  isLeftOpen = true;
  isRightOpen = true;
  verticalState = 0;
}


bool isOnLimitRight() {
  return !digitalRead(LIMIT_RIGHT);
}

bool isOnLimitLeft() {
  return !digitalRead(LIMIT_LEFT);
}

bool isOnLimitVertical() {
  return !digitalRead(LIMIT_VERTICAL);
}

void loop() {
  // float myTemp = getTemp();
  // Serial.print("TEMP ::");
  // Serial.println(myTemp);
  // Serial.print("RIGHT :: ");
  // Serial.print(LDRRight());
  // Serial.print("\t\tLEFT :: ");
  // Serial.println(LDRLeft());
  // int x = getIRValue();
  // if (x != -1) {
  //   Serial.print("IR :: ");
  //   Serial.println(x);
  // }
  // delay(1000);


  // Serial.print("RIGHT ::");
  // Serial.println(isOnLimitRight());
  // Serial.print("\t\tLEFT :: ");
  // Serial.print(isOnLimitLeft());
  // Serial.print("\t\tVERTICAL :: ");
  // Serial.println(isOnLimitVertical());



  // Serial << "IR :: " << getIRValue() << endl;
  // Serial << "RIGHT :: " << isOnLimitRight() << "\t\tLEFT :: " << isOnLimitLeft() << "\t\tVertical :: " << isOnLimitVertical << endl;

  // testing();
  // testing2();
  normalProccess();
}

void normalProccess() {

  if (timeChrono.hasPassed(1000)) {
    timeChrono.restart();
    myHour = myRTC.getHour(isAm, isH12);
  }

  int x = getIRValue();
  if (x == 28) {
    if (modeRight == 0 && modeLeft == 0 && modeVertical == 0) {
      isOnAutomatic = !isOnAutomatic;
      Serial.println(isOnAutomatic);
    }
  }

  if (isOnAutomatic) {
    LEDOff();
    if (sensorChrono.hasPassed(60000)) {
      sensorChrono.restart();


      int myLDRLeft = LDRLeft();
      int myLDRRight = LDRRight();
      float myTemp = getTemp();
      Serial.print("TEMP ::");
      Serial.println(myTemp);
      Serial.print("\t\tRIGHT :: ");
      Serial.print(myLDRRight);
      Serial.print("\t\tLEFT :: ");
      Serial.println(myLDRLeft);



      if ((myLDRLeft + myLDRRight) / 2 > 70) {
        RelayOff();
      } else if ((myLDRLeft + myLDRRight) / 2 > 50) {
        Serial.println("Switch 1");
        if (lightState == 0) {
          RelayOn();
          delay(500);
          lightState = 1;
        }
        switchLight(1 - lightState);
      } else if ((myLDRLeft + myLDRRight) / 2 > 30) {
        Serial.println("Switch 1");
        if (lightState == 0) {
          RelayOn();
          delay(500);
          lightState = 1;
        }
        switchLight(2 - lightState);
      } else {
        Serial.println("Switch 1");
        if (lightState == 0) {
          RelayOn();
          delay(500);
          lightState = 1;
        }
        switchLight(3 - lightState);
      }
      if (modeLeft == 0) {
        if (myLDRLeft > LDR_LEFT_LIMIT) {
          Serial.println("LEFT CLOSE");

          if (isLeftOpen) {
            modeLeft = 2;
            mytime = MOTOR_LEFT_CLOSE_DURATION;
            isLeftOpen = false;
            leftChrono.restart();
            Serial.print("time :: ");
            Serial.println(mytime);
          }
        } else {
          Serial.println("LEFT OPEN");

          if (!isLeftOpen) {
            modeLeft = 1;
            isLeftOpen = true;
          }
        }
      }


      if (modeRight == 0) {
        if (myLDRRight > LDR_RIGHT_LIMIT) {
          Serial.println("RIGHT CLOSE");

          if (isRightOpen) {
            modeRight = 2;
            mytime = MOTOR_RIGHT_CLOSE_DURATION;
            isRightOpen = false;
            rightChrono.restart();
            Serial.print("time :: ");
            Serial.println(mytime);
          }
        } else {
          Serial.println("RIGHT OPEN");

          if (!isRightOpen) {
            modeRight = 1;
            isRightOpen = true;
          }
        }
      }
      if (myHour >= 9 && myHour < 17) {
        if (modeVertical == 0) {
          if (myTemp > TEMP_UPPER_LIMIT) {
            // Serial.println("VERTICAL close");

            if (verticalState == 0) {
              modeVertical = 2;
              mytime = MOTOR_VERTICAL_1_3;
              verticalState = 2;
              Serial.print("time :: ");
              Serial.println(mytime);
              verticalChrono.restart();

            } else if (verticalState == 1) {
              modeVertical = 2;
              mytime = MOTOR_VERTICAL_2_3;
              verticalState = 2;
              Serial.print("time :: ");
              Serial.println(mytime);
              verticalChrono.restart();
            }
          } else if (myTemp < TEMP_UPPER_LIMIT && myTemp > TEMP_LOWER_LIMIT) {
            Serial.println("VERTICAL middle");

            if (verticalState == 0) {
              modeVertical = 2;
              mytime = MOTOR_VERTICAL_1_2;
              verticalState = 1;
              Serial.print("time :: ");
              Serial.println(mytime);
              verticalChrono.restart();

            } else if (verticalState == 2) {
              modeVertical = 3;
              mytime = MOTOR_VERTICAL_3_2;
              verticalState = 1;
              Serial.print("time :: ");
              Serial.println(mytime);
              verticalChrono.restart();
            }
          } else {

            Serial.println("VERTICAL open");
            if (verticalState != 0) {
              modeVertical = 1;
              verticalState = 0;
            }
          }
        }
      }



      //
    }

  } else {
    LEDOn();
    if (x == 8) {
      //left
      if (isLeftOpen) {
        Serial.println("LEFT CLOSE");
        modeLeft = 2;
        mytime = MOTOR_LEFT_CLOSE_DURATION;
        isLeftOpen = false;
        leftChrono.restart();
        Serial.print("time :: ");
        Serial.println(mytime);
      } else {
        Serial.println("LEFT OPEN");
        modeLeft = 1;
        isLeftOpen = true;
      }
      delay(1000);


    } else if (x == 90) {
      //right

      if (isRightOpen) {
        Serial.println("RIGHT CLOSE");

        modeRight = 2;
        mytime = MOTOR_RIGHT_CLOSE_DURATION;
        isRightOpen = false;
        rightChrono.restart();
        Serial.print("time :: ");
        Serial.println(mytime);
      } else {
        Serial.println("RIGHT OPEN");

        modeRight = 1;
        isRightOpen = true;
      }
      delay(1000);



    } else if (x == 24) {
      //up

      if (verticalState == 1) {
        Serial.println("VERTICAL open");
        if (verticalState != 0) {
          modeVertical = 1;
          verticalState = 0;
        }

      } else if (verticalState == 2) {
        Serial.println("VERTICAL middle");
        modeVertical = 3;
        mytime = MOTOR_VERTICAL_3_2;
        verticalState = 1;
        Serial.print("time :: ");
        Serial.println(mytime);
        verticalChrono.restart();
      }

      delay(1000);

    } else if (x == 82) {
      //down

      if (verticalState == 0) {
        Serial.println("VERTICAL middle");

        modeVertical = 2;
        mytime = MOTOR_VERTICAL_1_2;
        verticalState = 1;
        Serial.print("time :: ");
        Serial.println(mytime);
        verticalChrono.restart();

      } else if (verticalState == 1) {
        Serial.println("VERTICAL close");
        modeVertical = 2;
        mytime = MOTOR_VERTICAL_2_3;
        verticalState = 2;
        Serial.print("time :: ");
        Serial.println(mytime);
        verticalChrono.restart();
      }
      delay(1000);

    } else if (x == 69) {
      Serial.println("Switch Light");

      //1
      SwitchRelay();
      delay(1000);
    }
  }


  switch (modeLeft) {
    case 0:
      motorLeftOff();
      break;


    case 1:
      if (motorLeftBackward()) {
        modeLeft = 0;
      }
      break;

    case 2:
      motorLeftForward();
      if (leftChrono.hasPassed(mytime)) {
        modeLeft = 0;
      }
      break;
  }

  switch (modeRight) {
    case 0:
      motorRightOff();
      break;

    case 1:
      if (motorRightBackward()) {
        modeRight = 0;
      }
      break;

    case 2:
      motorRightForward();
      if (rightChrono.hasPassed(mytime)) {
        modeRight = 0;
      }
      break;
  }

  switch (modeVertical) {
    case 0:
      motorVerticalOff();
      break;

    case 1:
      if (motorVerticalBackward()) {
        modeVertical = 0;
      }
      break;

    case 2:
      motorVerticalForward();
      if (verticalChrono.hasPassed(mytime)) {
        modeVertical = 0;
      }
      break;

    case 3:
      motorVerticalBackward();
      if (verticalChrono.hasPassed(mytime)) {
        modeVertical = 0;
      }
      break;
  }
}

int LDRRight() {
  // return analogRead(LDR_RIGHT_PIN);
  return constrain(map(analogRead(LDR_RIGHT_PIN), 600, 0, 0, 100), 0, 100);
}

int LDRLeft() {
  // return analogRead(LDR_LEFT_PIN);
  return constrain(map(analogRead(LDR_LEFT_PIN), 500, 0, 0, 100), 0, 100);
}

void testing() {
  if (Serial.available()) {
    String reading = Serial.readString();
    // Serial << reading << endl;
    reading.trim();
    if (reading == "0") {
      Serial.println("Stop all");
      allOff();
    } else if (reading == "1") {
      Serial.println("Motor Left Forward");
      motorLeftForward();
    } else if (reading == "2") {
      Serial.println("Motor Left Backward");
      motorLeftBackward();
    } else if (reading == "3") {
      Serial.println("Motor Right Forward");
      motorRightForward();
    } else if (reading == "4") {
      Serial.println("Motor Right Backward");
      motorRightBackward();
    } else if (reading == "5") {
      Serial.println("Motor Vertical Forward");
      motorVerticalForward();
    } else if (reading == "6") {
      Serial.println("Motor Vertical Backward");
      motorVerticalBackward();
    } else if (reading == "7") {
      Serial.println("Relay On");
      RelayOn();
    } else if (reading == "q") {
      Serial.println("Switch 1");
      if (lightState == 0) {
        RelayOn();
        delay(500);
        lightState = 1;
      }
      switchLight(1 - lightState);
    } else if (reading == "w") {
      Serial.println("Switch 2");
      if (lightState == 0) {
        RelayOn();
        delay(500);
        lightState = 1;
      }
      switchLight(2 - lightState);
    } else if (reading == "e") {
      Serial.println("Switch 3");
      if (lightState == 0) {
        RelayOn();
        delay(500);
        lightState = 1;
      }
      switchLight(3 - lightState);
    }
  }
}


void testing2() {
  if (Serial.available()) {
    String reading = Serial.readString();
    reading.trim();
    if (reading == "0") {
      Serial.println("STOP ALL");
      testMode = 0;
    } else if (reading == "1") {
      Serial.println("Initialize");

      testMode = 1;
    } else if (reading == "2") {
      Serial.println("LEFT OPEN");

      if (!isLeftOpen) {
        testMode = 2;
        isLeftOpen = true;
      }

    } else if (reading == "3") {
      Serial.println("LEFT CLOSE");

      if (isLeftOpen) {
        testMode = 3;
        mytime = MOTOR_LEFT_CLOSE_DURATION;
        isLeftOpen = false;
        leftChrono.restart();
        Serial.print("time :: ");
        Serial.println(mytime);
      }
    } else if (reading == "4") {
      Serial.println("RIGHT OPEN");

      if (!isRightOpen) {
        testMode = 4;
        isRightOpen = true;
      }
    } else if (reading == "5") {
      Serial.println("RIGHT CLOSE");

      if (isRightOpen) {
        testMode = 5;
        mytime = MOTOR_RIGHT_CLOSE_DURATION;
        isRightOpen = false;
        rightChrono.restart();
        Serial.print("time :: ");
        Serial.println(mytime);
      }
    } else if (reading == "6") {
      Serial.println("VERTICAL open");

      if (verticalState != 0) {
        testMode = 6;
        verticalState = 0;
      }
    } else if (reading == "7") {
      Serial.println("VERTICAL middle");

      if (verticalState == 0) {
        testMode = 7;
        mytime = MOTOR_VERTICAL_1_2;
        verticalState = 1;
        Serial.print("time :: ");
        Serial.println(mytime);
        verticalChrono.restart();

      } else if (verticalState == 2) {
        testMode = 8;
        mytime = MOTOR_VERTICAL_3_2;
        verticalState = 1;
        Serial.print("time :: ");
        Serial.println(mytime);
        verticalChrono.restart();
      }
    } else if (reading == "8") {
      Serial.println("VERTICAL close");

      if (verticalState == 0) {
        testMode = 7;
        mytime = MOTOR_VERTICAL_1_3;
        verticalState = 2;
        Serial.print("time :: ");
        Serial.println(mytime);
        verticalChrono.restart();

      } else if (verticalState == 1) {
        testMode = 7;
        mytime = MOTOR_VERTICAL_2_3;
        verticalState = 2;
        Serial.print("time :: ");
        Serial.println(mytime);
        verticalChrono.restart();
      }
    }
  }
  switch (testMode) {
    case 0:
      allOff();
      break;

    case 1:
      if (initialize()) {
        testMode = 0;
        isLeftOpen = true;
        isRightOpen = true;
        verticalState = 0;
      }

      break;

    case 2:
      if (motorLeftBackward()) {
        testMode = 0;
      }
      break;

    case 3:
      motorLeftForward();
      if (leftChrono.hasPassed(mytime)) {
        testMode = 0;
      }
      break;

    case 4:
      if (motorRightBackward()) {
        testMode = 0;
      }
      break;

    case 5:
      motorRightForward();
      if (rightChrono.hasPassed(mytime)) {
        testMode = 0;
      }
      break;

    case 6:
      if (motorVerticalBackward()) {
        testMode = 0;
      }
      break;

    case 7:
      motorVerticalForward();
      if (verticalChrono.hasPassed(mytime)) {
        testMode = 0;
      }
      break;

    case 8:
      motorVerticalBackward();
      if (verticalChrono.hasPassed(mytime)) {
        testMode = 0;
      }
      break;
  }
}
void LEDOn() {
  digitalWrite(LED_PIN, HIGH);
}

void LEDOff() {
  digitalWrite(LED_PIN, LOW);
}
void RelayOn() {
  digitalWrite(RELAY_PIN, HIGH);
}
void RelayOff() {
  digitalWrite(RELAY_PIN, LOW);
}

void SwitchRelay() {
  if (digitalRead(RELAY_PIN)) {
    digitalWrite(RELAY_PIN, LOW);

  } else {
    digitalWrite(RELAY_PIN, HIGH);
  }
}
void allOff() {
  motorLeftOff();
  motorRightOff();
  motorVerticalOff();
  RelayOff();
}

void motorLeftOff() {
  digitalWrite(MOTOR_LEFT_A, LOW);
  digitalWrite(MOTOR_LEFT_B, LOW);
}

void motorLeftForward() {
  digitalWrite(MOTOR_LEFT_A, LOW);
  digitalWrite(MOTOR_LEFT_B, HIGH);
}


bool initialize() {
  bool left = motorLeftBackward();
  bool right = motorRightBackward();
  bool vertical = motorVerticalBackward();
  Serial.print("left ::");
  Serial.println(left);
  Serial.print("\t\tright :: ");
  Serial.print(right);
  Serial.print("\t\tVERTICAL :: ");
  Serial.print(vertical);
  Serial.print("\t\tALL :: ");
  Serial.println(left && right && vertical);
  return left && right && vertical;
}


bool motorLeftBackward() {
  if (isOnLimitLeft()) {
    digitalWrite(MOTOR_LEFT_A, LOW);
    digitalWrite(MOTOR_LEFT_B, LOW);
    return true;
  } else {
    digitalWrite(MOTOR_LEFT_A, HIGH);
    digitalWrite(MOTOR_LEFT_B, LOW);
    return false;
  }
}


void motorRightOff() {
  digitalWrite(MOTOR_RIGHT_A, LOW);
  digitalWrite(MOTOR_RIGHT_B, LOW);
}

void motorRightForward() {
  digitalWrite(MOTOR_RIGHT_A, HIGH);
  digitalWrite(MOTOR_RIGHT_B, LOW);
}

bool motorRightBackward() {
  if (isOnLimitRight()) {
    digitalWrite(MOTOR_RIGHT_A, LOW);
    digitalWrite(MOTOR_RIGHT_B, LOW);
    return true;
  } else {
    digitalWrite(MOTOR_RIGHT_A, LOW);
    digitalWrite(MOTOR_RIGHT_B, HIGH);
    return false;
  }
}

void motorVerticalOff() {
  digitalWrite(MOTOR_VERTICAL_A, LOW);
  digitalWrite(MOTOR_VERTICAL_B, LOW);
}

void motorVerticalForward() {
  digitalWrite(MOTOR_VERTICAL_A, HIGH);
  digitalWrite(MOTOR_VERTICAL_B, LOW);
}

bool motorVerticalBackward() {
  if (isOnLimitVertical()) {
    digitalWrite(MOTOR_VERTICAL_A, LOW);
    digitalWrite(MOTOR_VERTICAL_B, LOW);
    return true;
  } else {
    digitalWrite(MOTOR_VERTICAL_A, LOW);
    digitalWrite(MOTOR_VERTICAL_B, HIGH);
    return false;
  }
}


float getTemp() {
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  return event.temperature;
}

int getIRValue() {
  if (IRchrono.hasPassed(1000)) {
    IRchrono.restart();
    if (IrReceiver.decode()) {
      int x = IrReceiver.decodedIRData.command;
      IrReceiver.resume();
      return x;
    } else {
      return -1;
    }
  }
  return -1;
}

void switchLight(int state) {
  for (int i = 0; i < state; i++) {
    Serial.print("Pulse");
    RelayOff();
    delay(500);
    RelayOn();
    delay(500);
    lightState++;
  }
}
