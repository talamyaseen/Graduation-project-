#include <Arduino.h>
#include <Keypad.h>
#include <Servo.h>
#include <Stepper.h>
#include "max6675.h"

#define pushlimitSwitchUp 20
#define pushlimitSwitchDown 19
#define valveOpenPin 40
#define valveClosePin 41
#define directionPin 3
#define stepPin 25

#define stepsPerRevolution 6400 

#define railwaylimitSwitchForward 15
#define railwaylimitSwitchReverse 14   
#define railwayForward 4
#define railwayReverse 5  

#define mixerlimitSwitchUp 10
#define mixerlimitSwitchDown 18 
#define mixerOne 26
#define mixerTwo 27
#define mixerUp 7
#define mixerDown 6



// تعريف دبابيس مستشعرات IR
#define IR_SENSOR_1_PIN 16 // مستشعر IR الأول
#define IR_SENSOR_2_PIN 8  // مستشعر IR الثاني
#define RELAY_PIN 22       // دبوس التحكم بالموتور


const byte ROWS = 1; // صف واحد
const byte COLS = 2; // عمودان فقط (للزرين المستخدمين)


// تعريف دبابيس مستشعر الحرارة MAX6675
int ktcSO = 12;
int ktcCS = 11;
int ktcCLK = 13;

// تعريف دبابيس السخان
const int heaterPin = 23; // دبوس التحكم بالسخان

//pins nema17
// تعريف المنافذ المتصلة بـ H-Bridge
const int IN1 = 50;  // أول مدخل للمحرك
const int IN2 = 51;  // ثاني مدخل للمحرك
const int IN3 = 48; // ثالث مدخل للمحرك
const int IN4 = 49; // رابع مدخل للمحرك


char keys[ROWS][COLS] = {
  {'1', '2'} // الزرين المستخدمين
};

byte pin_rows[ROWS] = {32};       // الدبوس الموصل بالصف
byte pin_column[COLS] = {31,30};  // الدبابيس الموصلة بالعمودين للزرين
Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROWS, COLS);


// تعريف السيرفو
Servo myServo;
// تعريف السيرفو
Servo myServoUP;    // سيرفو لحركة رفع البسكويت
Servo myServoDown;  // سيرفو لتنزيل البسكويت


volatile bool systemStart = true;  
volatile bool stopForward = false;  
volatile bool stopReverse = false;  

volatile bool stopMixerUP = false;  
volatile bool stopMixerDown = false;  
bool isMixing = false;


bool isCocoaDone = false;


// تعريف أعلام التحكم
bool creamFlag = false;       // علم خاص بالكريمة
//bool chocolateFlag = false;   // علم خاص بالشوكولاتة
bool biscuitDropped = false; // علم لتنزيل البسكويت مرة واحدة
bool creamIR=false;
bool stepperC=false;
bool up=false;
#define mixerStartTime 6000  // 6 seconds in milliseconds (change to 1200000 for 20 minutes)

unsigned long mixerStartMillis = 0;
bool isMixerRunning = false;


#define STEPS_PER_REV 200

// تهيئة مكتبة MAX6675
MAX6675 ktc(ktcCLK, ktcCS, ktcSO);

//nema23
// تهيئة مكتبة Stepper مع دبابيس H-Bridge المتصلة بالمحرك الخطوي
Stepper myStepper(STEPS_PER_REV, 46, 47, 44, 45);
// إنشاء كائن للتحكم بالمحرك
//nema17
Stepper stepper(STEPS_PER_REV, IN1, IN2, IN3, IN4);

const int heatTime = 10000; // مدة تشغيل السخان (بالمللي ثانية)

void forwardFunction() {
  digitalWrite(railwayForward , HIGH);
  digitalWrite(railwayReverse, LOW);  
  Serial.println("Moving forward...");
}

void reverseFunction() {
  digitalWrite(railwayForward , LOW);  
  digitalWrite(railwayReverse, HIGH); 
  Serial.println("Moving in reverse...");
}

void stopRailwayMovement() {
  digitalWrite(railwayForward, HIGH);
  digitalWrite(railwayReverse, HIGH);
  Serial.println("Movement stopped.");
}

void mixerMoveUp() {
  Serial.println("Mixer Motor moving up.");
  digitalWrite(mixerUp , LOW);  
  digitalWrite(mixerDown, HIGH);
}

void mixerMoveDown() {
  Serial.println("Mixer Motor moving down.");
  digitalWrite(mixerDown , LOW);  
  digitalWrite(mixerUp, HIGH);
}

void stopMixersMotor() {
  Serial.println("Mixers Motor stopped.");
  digitalWrite(mixerDown , HIGH);  
  digitalWrite(mixerUp, HIGH);
}

void startMixers() {
  Serial.println("Mixers started.");
  digitalWrite(mixerOne , LOW);  
  digitalWrite(mixerTwo, LOW);
  mixerStartMillis = millis();  // Store the time when mixers start
  isMixerRunning = true;
}

void stopMixers() {
  Serial.println("Mixers stopped.");
  digitalWrite(mixerOne , HIGH);  
  digitalWrite(mixerTwo, HIGH);
}

bool valveOpen() {
  digitalWrite(valveOpenPin,LOW);
  digitalWrite(valveClosePin, HIGH);
  Serial.println("Valve opening.");

}

bool valveClose() {
  digitalWrite(valveClosePin,LOW);
  digitalWrite(valveOpenPin, HIGH);
  Serial.println("Valve closing");

}

void stopValveMovement() {
  digitalWrite(valveClosePin,HIGH);
  digitalWrite(valveOpenPin, HIGH);
  Serial.println("Valve Stoped");
}

void coacoAdding(char key){
 
  if (key) {
    Serial.print("Key pressed: ");
    Serial.println(key);
    
    if (key == '1') {            // إذا تم الضغط على الزر 1
      myServo.write(0);         // تحريك السيرفو إلى 0 درجة
      delay(2000);              // الانتظار لمدة ثانيتين
      myServo.write(90);        // إعادة السيرفو إلى الوضع الأول (90 درجة)
    } else if (key == '2') {     // إذا تم الضغط على الزر 2
      myServo.write(180);       // تحريك السيرفو إلى 180 درجة
      delay(2000);              // الانتظار لمدة ثانيتين
      myServo.write(90);        // إعادة السيرفو إلى الوضع الأول (90 درجة)
    }
  }

}

void moveStepper(int direction, int speed, int steps) {
  digitalWrite(directionPin, direction); // Set the direction of the stepper

  int delayTime = speed; // Convert speed (steps per second) to delay in microseconds

  for (int i = 0; i < steps; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(delayTime / 2); // Half the delay for HIGH state
    digitalWrite(stepPin, LOW);
    delayMicroseconds(delayTime / 2); // Half the delay for LOW state
  }
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
      digitalWrite(RELAY_PIN, HIGH);
  }
}
void handleChocolate() {
  double tempC = ktc.readCelsius();
  Serial.print("Chocolate Temperature (C): ");
  Serial.println(tempC);

  //digitalWrite(heaterPin, HIGH);
  //delay(heatTime);
  //digitalWrite(heaterPin, LOW);

  Serial.println("Chocolate heated. Opening nozzle...");
  for (int i = 0; i < 7; i++) {
    myStepper.step(STEPS_PER_REV); // فتح
     stepper.step(STEPS_PER_REV);
  }
  delay(5000);
  for (int i = 0; i < 7; i++) {
    myStepper.step(-STEPS_PER_REV); // إغلاق
     stepper.step(STEPS_PER_REV);
  }

  Serial.println("Nozzle closed.");
}

void setup() {
  Serial.begin(9600);  

  pinMode(railwaylimitSwitchForward, INPUT_PULLUP); 
  pinMode(railwaylimitSwitchReverse, INPUT_PULLUP); 
  pinMode(railwayForward, OUTPUT);          
  pinMode(railwayReverse, OUTPUT);         

  pinMode(mixerlimitSwitchUp, INPUT_PULLUP); 
  pinMode(mixerlimitSwitchDown, INPUT_PULLUP); 
  pinMode(mixerOne, OUTPUT);          
  pinMode(mixerTwo, OUTPUT); 
  pinMode(mixerUp, OUTPUT);          
  pinMode(mixerDown, OUTPUT); 


 pinMode(pushlimitSwitchUp, INPUT_PULLUP);
    pinMode(pushlimitSwitchDown, INPUT_PULLUP);
   
    // Initialize output pins
    pinMode(valveOpenPin, OUTPUT);
    pinMode(valveClosePin, OUTPUT);
    pinMode(directionPin, OUTPUT);
    pinMode(stepPin, OUTPUT);

  digitalWrite(mixerOne, HIGH); 
  digitalWrite(mixerTwo, HIGH);

 digitalWrite(valveClosePin,HIGH);
  digitalWrite(valveOpenPin, HIGH);
   myServo.attach(33);             // توصيل السيرفو إلى الدبوس D9
  myServo.write(90);   

 digitalWrite(RELAY_PIN, LOW);

   myServoUP.attach(37);
  myServoDown.attach(36);
  myServoUP.write(72);
  myServoDown.write(100);
  pinMode(IR_SENSOR_1_PIN, INPUT);
  pinMode(IR_SENSOR_2_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);

  pinMode(heaterPin, OUTPUT);
  digitalWrite(heaterPin, HIGH);
  myStepper.setSpeed(30);

//nema17
   stepper.setSpeed(100);
  //nema17
  // إعداد المنافذ كإخراج
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
}

static bool startupComplete = false;
char key='T';
void loop() {
    while(key!='1'&&key!='2'){
     key=keypad.getKey();    // قراءة الزر المضغوط
      Serial.println(key);
    //mixerMoveUp();
    //forwardFunction();
    }

 if (!startupComplete) {
    Serial.println("Startup complete. Waiting for input...");
    startupComplete = true;
            // valveClose();
             //delay(5000);
             //stopValveMovement();
     reverseFunction();
    // delay(100000);
     delay(5000);
  }

 if (digitalRead(railwaylimitSwitchForward) == LOW) {  
  if(up==true){
    stopRailwayMovement();
    Serial.println("Forward limit switch pressed. Stopping forward movement.");
     valveOpen();
     delay(5000);
     stopValveMovement();
   for(int i=0;i<55;i++)
    moveStepper(LOW, 70, stepsPerRevolution);
       while (digitalRead(pushlimitSwitchDown) == HIGH) {
      moveStepper(LOW, 300, stepsPerRevolution); 
       }
    creamFlag=true;
    up=false;
  }
  }

  
 if (digitalRead(pushlimitSwitchDown) == LOW) {  
  if(creamIR==true&&isCocoaDone==true){
    creamIR=false;
    isCocoaDone=false;
    for(int i=0;i<65;i++)
    moveStepper(HIGH, 300, stepsPerRevolution);
    
    
   while (digitalRead(pushlimitSwitchUp) == HIGH) {
      moveStepper(HIGH, 70, stepsPerRevolution);
    }
     reverseFunction();
   delay(5000);
   //stopRailwayMovement();
   Serial.println("Forword.");
  }
 }

  

  if (digitalRead(railwaylimitSwitchReverse) == LOW) {  // Reverse switch pressed
    Serial.println("Reverse limit switch pressed. Stopping reverse movement.");
    forwardFunction();
    delay(1500);
    stopRailwayMovement();

  
    coacoAdding(key);
    mixerMoveDown();  // Start moving the mixer down when reverse button is pressed
  }

  if (digitalRead(mixerlimitSwitchUp) == LOW) {  // Mixer Up switch pressed
    mixerMoveDown();
    delay(1500);
    stopMixersMotor();
    forwardFunction();  // Start forward movement when mixerUp is pressed
      dropBiscuit();
      up=true;

  }

  if (digitalRead(mixerlimitSwitchDown) == LOW) {  // Mixer Down switch pressed
    mixerMoveUp();
    delay(1500);
    stopMixersMotor();
    if (!isMixerRunning) {
      startMixers();  // Start mixers when mixerDown switch is pressed
    }
  }

  // If mixers are running and 20 minutes have passed (change to 1200000 ms for 20 minutes)
  if (isMixerRunning && (millis() - mixerStartMillis >= mixerStartTime)) {
    stopMixers();  // Stop the mixers after 20 minutes
    mixerMoveUp();  // Move the mixer up after 20 minutes
      Serial.println("test test");
    stepperC=true;
    delay(5000);
    isMixerRunning = false;  // Stop the mixer running state
  }
  
    if (digitalRead(IR_SENSOR_1_PIN) == LOW) {
    digitalWrite(RELAY_PIN, LOW);
    Serial.println("Biscuit detected at IR sensor 1. Waiting for cream...");

    if (creamFlag) {
      Serial.println("Cream added. Resuming line...");
      creamFlag = false;
      delay(500);
      digitalWrite(RELAY_PIN, HIGH);
    //  delay(3000);
      creamIR=true;
    }
  }

  if (digitalRead(IR_SENSOR_2_PIN) == LOW) {
    digitalWrite(RELAY_PIN, LOW);
    Serial.println("Biscuit detected at IR sensor 2. Waiting for chocolate...");

   // if (chocolateFlag) {
      handleChocolate();
     // chocolateFlag = false;
      isCocoaDone =true;
       stepperC=false;
      delay(500);
      digitalWrite(RELAY_PIN, HIGH);
      delay(3000);
   // }
  }

  if(stepperC){
        stepper.step(STEPS_PER_REV);
  }

}


