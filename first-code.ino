#include <Arduino.h>
#include <Keypad.h>
#include <Servo.h>
#define directionPin 28
#define stepPin 29

#define stepsPerRevolution 6400 

#define railwaylimitSwitchForward 18   
#define railwaylimitSwitchReverse  21   
#define railwayForward 38    
#define railwayReverse 39  

#define mixerlimitSwitchUp 3
#define mixerlimitSwitchDown  19 
#define mixerOne 24
#define mixerTwo 25
#define mixerUp 26
#define mixerDown 27

const byte ROWS = 2; 
const byte COLS = 1; 

char keys[ROWS][COLS] = {
  {'1'},   
  {'2'}
};

byte pin_rows[ROWS] = {4, 5};    
byte pin_column[COLS] = {};    
Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROWS, COLS);
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
  digitalWrite(mixerUp , HIGH);  
  digitalWrite(mixerDown, LOW);
}

void mixerMoveDown() {
  Serial.println("Mixer Motor moving down.");
  digitalWrite(mixerDown , HIGH);  
  digitalWrite(mixerUp, LOW);
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
  mixerStartMillis = millis();  
  isMixerRunning = true;
}

void stopMixers() {
  Serial.println("Mixers stopped.");
  digitalWrite(mixerOne , HIGH);  
  digitalWrite(mixerTwo, HIGH);
}

int cocoaAdding (){
  int choose;
  char key = keypad.getKey();    
  if (key) {
    Serial.print("Key pressed: ");
    Serial.println(key);
    
    if (key == '1') {           
      myServo.write(0);         
      delay(2000);              
      myServo.write(90);  
     choose=0;
    }
    else if(key == '2'){
        choose=1;
    }

  }
  else  choose=2;
}

void setup() {
  Serial.begin(9600);  
  //cocoa
  myServo.attach(7);           
  Serial.begin(9600);
  delay(2000);
  myServo.write(90);         

  pinMode(directionPin, OUTPUT);
  pinMode(stepPin, OUTPUT);

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

  digitalWrite(mixerOne, HIGH); 
  digitalWrite(mixerTwo, HIGH);
}

void loop() {

 
 if (digitalRead(railwaylimitSwitchForward) == LOW) { 
    Serial.println("Forward limit switch pressed. Stopping forward movement.");
    reverseFunction(); 
  }

  if (digitalRead(railwaylimitSwitchReverse) == LOW) {  
    Serial.println("Reverse limit switch pressed. Stopping reverse movement.");
    forwardFunction();
    delay(1000);
    stopRailwayMovement();
    mixerMoveDown();  
  }

  if (digitalRead(mixerlimitSwitchUp) == LOW) {  
    mixerMoveDown();
    delay(1000);
    stopMixersMotor();
    forwardFunction(); 
  }

  if (digitalRead(mixerlimitSwitchDown) == LOW) { 
    mixerMoveUp();
    delay(1000);
    stopMixersMotor();
    if (!isMixerRunning) {
      startMixers(); 
    }
  }

  
  if (isMixerRunning && (millis() - mixerStartMillis >= mixerStartTime)) {
    stopMixers(); 
    mixerMoveUp();  
    isMixerRunning = false; 
  }
  

}
