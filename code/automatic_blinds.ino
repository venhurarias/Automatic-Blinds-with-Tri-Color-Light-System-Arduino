#include <Chrono.h>
#include <Streaming.h>
#include <IRremote.h>
#include <IRremoteInt.h>
#include <DHT.h>

#define DHT_PIN 2
#define LDR_LEFT_PIN A0
#define LDR_RIGHT_PIN A1
#define MOTOR_LEFT_A 3
#define MOTOR_LEFT_B 4
#define MOTOR_RIGHT_A 5
#define MOTOR_RIGHT_B 6
#define MOTOR_VERTICAL_A 7
#define MOTOR_VERTICAL_B 8
#define LIMIT_RIGHT 9
#define LIMIT_LEFT 10
#define LIMIT_VERTICAL 11
#define IR_PIN 13
#define DHTTYPE DHT22

IRrecv receiver(IR_PIN);
decode_results results;
DHT dht(DHT_PIN, DHTTYPE);

Chrono myChrono;
int testMode, mode;

int leftState;
int rightState;
int verticalState;
long time;
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
  dht.begin();
  receiver.enableIRIn();
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
  // Serial << "IR :: " << getIRValue() << endl;
  // Serial << "TEMP :: " << getTemp() << endl;
  Serial << "RIGHT :: " << isOnLimitRight() << "\t\tLEFT :: " << isOnLimitLeft() << "\t\tVertical :: " << isOnLimitVertical << endl;

  // testing();
}


void testing() {
  if (Serial.available()) {
    String reading = Serial.readString();
    Serial << reading << endl;
    reading.trim();
    if (reading == "0") {
      Serial << "STOP ALL" << endl;
      allOff();
    } else if (reading == "1") {
      Serial << "Motor Left Forward" << endl;
      motorLeftForward();
    } else if (reading == "2") {
      Serial << "Motor Left Backward" << endl;
      motorLeftBackward();
    } else if (reading == "3") {
      Serial << "Motor Right Forward" << endl;
      motorRightForward();
    } else if (reading == "4") {
      Serial << "Motor Right Backward" << endl;
      motorRightBackward();
    } else if (reading == "5") {
      Serial << "Motor Vertical Forward" << endl;
      motorVerticalForward();
    } else if (reading == "6") {
      Serial << "Motor Vertical Backward" << endl;
      motorVerticalBackward();
    }
  }
}


void testing2() {
  if (Serial.available()) {
    String reading = Serial.readString();
    Serial << reading << endl;
    reading.trim();
    if (reading == "0") {
      Serial << "STOP ALL" << endl;
      testMode = 0;
    } else if (reading == "w") {
      Serial << "Motor Up" << endl;
      testMode = 1;
      myChrono.restart();
    } else if (reading == "s") {
      Serial << "Motor Down" << endl;
      testMode = 2;
    } else if (reading == 'a') {
      Serial << "Motor Left Switch" << endl;
      testMode = 2;
    } else if (reading == 'd') {
      Serial << "Motor Right Switch" << endl;
      testMode = 2;
    }
  }
  switch (testMode) {
    case 0:
      allOff();
      break;

    case 1:
      if (myChrono.hasPassed(time)) {
        motorLeftForward();
      }
      mode = 0;
      break;

    case 2:
      if (myChrono.hasPassed(time)) {
        motorLeftBackward();
      }
      mode = 0;
      break;

    case 3:
      if (myChrono.hasPassed(time)) {
        motorRightForward();
      }
      mode = 0;
      break;

    case 4:
      if (myChrono.hasPassed(time)) {
        motorRightBackward();
      }
      mode = 0;
      break;

    case 3:
      if (myChrono.hasPassed(time)) {
        motorVerticalForward();
      }
      mode = 0;
      break;

    case 4:
      if (myChrono.hasPassed(time)) {
        motorVerticalBackward();
      }
      mode = 0;
      break;
  }
}
void allOff() {
  motorLeftOff();
  motorRightOff();
  motorVerticalOff();
}

void motorLeftOff() {
  digitalWrite(MOTOR_LEFT_A, LOW);
  digitalWrite(MOTOR_LEFT_A, LOW);
}

void motorLeftForward() {
  digitalWrite(MOTOR_LEFT_A, HIGH);
  digitalWrite(MOTOR_LEFT_A, LOW);
}

void motorLeftBackward() {
  digitalWrite(MOTOR_LEFT_A, LOW);
  digitalWrite(MOTOR_LEFT_A, HIGH);
}


void motorRightOff() {
  digitalWrite(MOTOR_RIGHT_A, LOW);
  digitalWrite(MOTOR_RIGHT_A, LOW);
}

void motorRightForward() {
  digitalWrite(MOTOR_RIGHT_A, HIGH);
  digitalWrite(MOTOR_RIGHT_A, LOW);
}

void motorRightBackward() {
  digitalWrite(MOTOR_RIGHT_A, LOW);
  digitalWrite(MOTOR_RIGHT_A, HIGH);
}

void motorVerticalOff() {
  digitalWrite(MOTOR_VERTICAL_A, LOW);
  digitalWrite(MOTOR_VERTICAL_B, LOW);
}

void motorVerticalForward() {
  digitalWrite(MOTOR_VERTICAL_A, HIGH);
  digitalWrite(MOTOR_VERTICAL_B, LOW);
}

void motorVerticalBackward() {
  digitalWrite(MOTOR_VERTICAL_A, LOW);
  digitalWrite(MOTOR_VERTICAL_B, HIGH);
}



float getTemp() {
  return dht.readTemperature();
}

int getIRValue() {
  if (receiver.decode(&results)) {
    return (int)results.value;
    receiver.resume();
  } else {
    return -1;
  }
}
