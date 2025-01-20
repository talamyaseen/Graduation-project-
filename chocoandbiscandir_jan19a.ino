#include <Servo.h>
#include <Stepper.h>
#include "max6675.h"

// تعريف السيرفو
Servo myServoUP;    // سيرفو لحركة رفع البسكويت
Servo myServoDown;  // سيرفو لتنزيل البسكويت

// تعريف دبابيس مستشعرات IR
#define IR_SENSOR_1_PIN 16 // مستشعر IR الأول
#define IR_SENSOR_2_PIN 8  // مستشعر IR الثاني
#define RELAY_PIN 22       // دبوس التحكم بالموتور

// تعريف أعلام التحكم
bool creamFlag = true;       // علم خاص بالكريمة
bool chocolateFlag = true;   // علم خاص بالشوكولاتة
bool biscuitDropped = false; // علم لتنزيل البسكويت مرة واحدة

// تعريف دبابيس السخان
const int heaterPin = 23; // دبوس التحكم بالسخان

// تعريف دبابيس مستشعر الحرارة MAX6675
int ktcSO = 12;
int ktcCS = 11;
int ktcCLK = 13;

// عدد الخطوات لكل دورة كاملة للمحرك الخطوي
#define STEPS_PER_REV 200

// تهيئة مكتبة MAX6675
MAX6675 ktc(ktcCLK, ktcCS, ktcSO);

// تهيئة مكتبة Stepper مع دبابيس H-Bridge المتصلة بالمحرك الخطوي
Stepper myStepper(STEPS_PER_REV, 46, 47, 44, 45);

const int heatTime = 10000; // مدة تشغيل السخان (بالمللي ثانية)

// إعداد النظام
void setup() {
  // إعداد السيرفو
  myServoUP.attach(37);
  myServoDown.attach(36);
  myServoUP.write(72);
  myServoDown.write(100);
  pinMode(IR_SENSOR_1_PIN, INPUT);
  pinMode(IR_SENSOR_2_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);
  pinMode(heaterPin, OUTPUT);
  myStepper.setSpeed(30);
  Serial.begin(9600);
  Serial.println("System Initialized");
}
void dropBiscuit() {
  if (!biscuitDropped) {
    myServoUP.write(0);
    delay(200);
    myServoUP.write(72);
    delay(2000);

    myServoDown.write(0);
    delay(500);
    myServoDown.write(100);
    delay(800);

    Serial.println("Biscuit dropped.");
    biscuitDropped = true;
  }
}
void handleChocolate() {
  double tempC = ktc.readCelsius();
  Serial.print("Chocolate Temperature (C): ");
  Serial.println(tempC);

  digitalWrite(heaterPin, HIGH);
  delay(heatTime);
  digitalWrite(heaterPin, LOW);

  Serial.println("Chocolate heated. Opening nozzle...");
  for (int i = 0; i < 7; i++) {
    myStepper.step(-STEPS_PER_REV); // فتح
  }
  delay(5000);
  for (int i = 0; i < 7; i++) {
    myStepper.step(STEPS_PER_REV); // إغلاق
  }
  Serial.println("Nozzle closed.");
}

void loop() {
  dropBiscuit();

  if (digitalRead(IR_SENSOR_1_PIN) == LOW) {
    digitalWrite(RELAY_PIN, LOW);
    Serial.println("Biscuit detected at IR sensor 1. Waiting for cream...");

    if (creamFlag) {
      Serial.println("Cream added. Resuming line...");
      creamFlag = false;
      delay(500);
      digitalWrite(RELAY_PIN, HIGH);
      delay(3000);
    }
  }

  if (digitalRead(IR_SENSOR_2_PIN) == LOW) {
    digitalWrite(RELAY_PIN, LOW);
    Serial.println("Biscuit detected at IR sensor 2. Waiting for chocolate...");

    if (chocolateFlag) {
      handleChocolate();
      chocolateFlag = false;
      delay(500);
      digitalWrite(RELAY_PIN, HIGH);
      delay(3000);
    }
  }

  if (digitalRead(IR_SENSOR_1_PIN) == HIGH && !creamFlag) {
    creamFlag = true;
    Serial.println("Cream flag reset.");
  }

  if (digitalRead(IR_SENSOR_2_PIN) == HIGH && !chocolateFlag) {
    chocolateFlag = true;
    Serial.println("Chocolate flag reset.");
  }

  delay(100);
}
