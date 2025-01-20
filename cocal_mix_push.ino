#include <Arduino.h>
#include <Keypad.h>
#include <Servo.h>

#define pushlimitSwitchUp 20
#define pushlimitSwitchDown 19
#define valveOpenPin 40
#define valveClosePin 41
#define directionPin 25
#define stepPin 24

#define stepsPerRevolution 6400 

#define railwaylimitSwitchForward 15
#define railwaylimitSwitchReverse 14   
#define railwayForward 4
#define railwayReverse 3  

#define mixerlimitSwitchUp 21
#define mixerlimitSwitchDown 18 
#define mixerOne 26
#define mixerTwo 27
#define mixerUp 7
#define mixerDown 6

const byte ROWS = 1; // صف واحد
const byte COLS = 2; // عمودان فقط (للزرين المستخدمين)

char keys[ROWS][COLS] = {
  {'1', '2'} // الزرين المستخدمين
};

byte pin_rows[ROWS] = {32};       // الدبوس الموصل بالصف
byte pin_column[COLS] = {31,30};  // الدبابيس الموصلة بالعمودين للزرين
Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROWS, COLS);

// تعريف السيرفو
Servo myServo;

volatile bool systemStart = true;  
volatile bool stopForward = false;  
volatile bool stopReverse = false;  

volatile bool stopMixerUP = false;  
volatile bool stopMixerDown = false;  
bool isMixing = false;

#define mixerStartTime 6000  // 6 seconds in milliseconds (change to 1200000 for 20 minutes)

unsigned long mixerStartMillis = 0;
bool isMixerRunning = false;

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

/*
int movePushMotor(int direction, int step, long int steps, bool isUp, int speed) {
  // Set the motor direction
  digitalWrite(direction, isUp ? HIGH : LOW); // LOW for up (CW), HIGH for down (CCW)

  for (long int i = 0; i < steps; i++) {
    // Check the respective limit switch
    if (isUp && digitalRead(pushlimitSwitchUp) == LOW) {
      Serial.println("Upper limit switch triggered, stopping motor.");
      return -1; // Stop motor movement return -1 means the operation done
    } else if (!isUp && digitalRead(pushlimitSwitchDown) == LOW) {
      Serial.println("Lower limit switch triggered, stopping motor.");
      return 1; // Stop motor movement means the opertation done and the cream is empty-move up
    }

    // Generate a step pulse 
    digitalWrite(step, HIGH);
    delayMicroseconds(speed); // Adjust delay for speed
    digitalWrite(step, LOW);
    delayMicroseconds(speed); // Adjust delay for speed
  }
}
*/
/*void handelPushMovement(){
  //  movePushMotor(directionPin,stepPin,200000,false,70);
 movePushMotor(directionPin,stepPin,200000,false,300);
    movePushMotor(directionPin,stepPin,200000,true,300);
    //movePushMotor(directionPin,stepPin,20000,true,70);

}*/


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

  pinMode(valveOpenPin, OUTPUT);
pinMode(valveClosePin, OUTPUT);
pinMode(directionPin, OUTPUT);
pinMode(stepPin, OUTPUT);

 pinMode(pushlimitSwitchUp, INPUT_PULLUP);
    pinMode(pushlimitSwitchDown, INPUT_PULLUP);
   
    // Initialize output pins
    pinMode(valveOpenPin, OUTPUT);
    pinMode(valveClosePin, OUTPUT);
    pinMode(directionPin, OUTPUT);
    pinMode(stepPin, OUTPUT);

  digitalWrite(mixerOne, HIGH); 
  digitalWrite(mixerTwo, HIGH);

   myServo.attach(33);             // توصيل السيرفو إلى الدبوس D9
  myServo.write(90);   
}

static bool startupComplete = false;
char key='T';
void loop() {

 if (!startupComplete) {
    Serial.println("Startup complete. Waiting for input...");
    startupComplete = true;
    delay(100); // Ensure stable readings
      reverseFunction();
      delay(6000);
  }

 if (digitalRead(railwaylimitSwitchForward) == LOW) {  

    stopRailwayMovement();
  // Check for limit switch presses
// Forward switch pressed
    Serial.println("Forward limit switch pressed. Stopping forward movement.");
 //   reverseFunction();  // Call reverse function when forward button is pressed
     valveOpen();
     delay(5000);
     stopValveMovement();
   for(int i=0;i<65;i++)
    moveStepper(LOW, 70, stepsPerRevolution);
   
    // Move down at speed 300 for one full revolution
    //moveStepper(LOW, 300, stepsPerRevolution);
  Serial.println("Fast1 end .");
    // Continue moving down at speed 300 until the lower limit switch is pressed
    while (digitalRead(pushlimitSwitchDown) == HIGH) {
      moveStepper(LOW, 300, stepsPerRevolution); // Small steps to keep checking the switch
    }

    // Change direction to up
    // Move up at speed 300 for one full revolution
    for(int i=0;i<65;i++)
    moveStepper(HIGH, 300, stepsPerRevolution);
    // Continue moving up at speed 70 until the upper limit switch is pressed
    
   while (digitalRead(pushlimitSwitchUp) == HIGH) {
      moveStepper(HIGH, 70, stepsPerRevolution); // Small steps to keep checking the switch
    }
     reverseFunction();

   Serial.println("Forword.");


  }

  if (digitalRead(railwaylimitSwitchReverse) == LOW) {  // Reverse switch pressed
    Serial.println("Reverse limit switch pressed. Stopping reverse movement.");
    forwardFunction();
    delay(1000);
    stopRailwayMovement();
    while(true){
     key=keypad.getKey();    // قراءة الزر المضغوط
     if(key=='1'||key=='2')
     break;
     Serial.println(key);
    }
    coacoAdding(key);
    mixerMoveDown();  // Start moving the mixer down when reverse button is pressed
  }

  if (digitalRead(mixerlimitSwitchUp) == LOW) {  // Mixer Up switch pressed
    mixerMoveDown();
    delay(1000);
    stopMixersMotor();
    forwardFunction();  // Start forward movement when mixerUp is pressed
  }

  if (digitalRead(mixerlimitSwitchDown) == LOW) {  // Mixer Down switch pressed
    mixerMoveUp();
    delay(1000);
    stopMixersMotor();
    if (!isMixerRunning) {
      startMixers();  // Start mixers when mixerDown switch is pressed
    }
  }

  // If mixers are running and 20 minutes have passed (change to 1200000 ms for 20 minutes)
  if (isMixerRunning && (millis() - mixerStartMillis >= mixerStartTime)) {
    stopMixers();  // Stop the mixers after 20 minutes
    mixerMoveUp();  // Move the mixer up after 20 minutes
    isMixerRunning = false;  // Stop the mixer running state
  }
  
  // Add any other logic needed for your system
}


