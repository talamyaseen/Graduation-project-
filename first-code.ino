#include <Arduino.h>
#include <Keypad.h>
#include <Servo.h>

#define directionPin 28
#define stepPin 29

#define stepsPerRevolution 6400 

#define railwaylimitSwitchForward 18   
#define railwaylimitSwitchReverse 21   
#define railwayForward 38    
#define railwayReverse 39  

#define mixerlimitSwitchUp 3
#define mixerlimitSwitchDown 19 
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

volatile bool isMixerRunning = false;
unsigned long mixerStartMillis = 0;
const unsigned long mixerRunDuration = 6000; // 6 seconds

void setup() {
  Serial.begin(9600);
  myServo.attach(7);
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
  if (!handleRailwayMovement()) {
    Serial.println("Error handling railway movement.");
  }

  if(cocoaAdding()==1||cocoaAdding()==0)
  if (!handleMixerMovement()) {
    Serial.println("Error handling mixer movement.");
  }
}

bool handleRailwayMovement() {
  if (digitalRead(railwaylimitSwitchForward) == LOW) {
    Serial.println("Forward limit switch pressed.");
    if (!reverseFunction()) {
      Serial.println("Failed to execute reverse function.");
      return false;
    }
  } else if (digitalRead(railwaylimitSwitchReverse) == LOW) {
    Serial.println("Reverse limit switch pressed.");
    if (!forwardFunction()) {
      Serial.println("Failed to execute forward function.");
      return false;
    }
    delay(1000);
    stopRailwayMovement();
    if (!mixerMoveDown()) {
      Serial.println("Failed to move mixer down.");
      return false;
    }
  }
  return true;
}

bool handleMixerMovement() {

  if (digitalRead(mixerlimitSwitchUp) == LOW) {
    Serial.println("Mixer limit switch up activated.");
    if (!mixerMoveDown()) {
      Serial.println("Failed to move mixer down.");
      return false;
    }
    delay(1000);
    stopMixersMotor();
    if (!forwardFunction()) {
      Serial.println("Failed to execute forward function.");
      return false;
    }
  } else if (digitalRead(mixerlimitSwitchDown) == LOW) {
    Serial.println("Mixer limit switch down activated.");
    if (!mixerMoveUp()) {
      Serial.println("Failed to move mixer up.");
      return false;
    }
    delay(1000);
    stopMixersMotor();
    if (!isMixerRunning) {
      if (!startMixers()) {
        Serial.println("Failed to start mixers.");
        return false;
      }
    }
  }

  if (isMixerRunning && (millis() - mixerStartMillis >= mixerRunDuration)) {
    if (!stopMixers()) {
      Serial.println("Failed to stop mixers.");
      return false;
    }
    if (!mixerMoveUp()) {
      Serial.println("Failed to move mixer up.");
      return false;
    }
    isMixerRunning = false;
  }
  return true;
}

bool forwardFunction() {
  digitalWrite(railwayForward, HIGH);
  digitalWrite(railwayReverse, LOW);
  Serial.println("Moving forward...");
  return true;
}

bool reverseFunction() {
  digitalWrite(railwayForward, LOW);
  digitalWrite(railwayReverse, HIGH);
  Serial.println("Moving reverse...");
  return true;
}

void stopRailwayMovement() {
  digitalWrite(railwayForward, HIGH);
  digitalWrite(railwayReverse, HIGH);
  Serial.println("Railway stopped.");
}

bool mixerMoveUp() {
  digitalWrite(mixerUp, HIGH);
  digitalWrite(mixerDown, LOW);
  Serial.println("Mixer moving up.");
  return true;
}

bool mixerMoveDown() {
  digitalWrite(mixerDown, HIGH);
  digitalWrite(mixerUp, LOW);
  Serial.println("Mixer moving down.");
  return true;
}

void stopMixersMotor() {
  digitalWrite(mixerDown, HIGH);
  digitalWrite(mixerUp, HIGH);
  Serial.println("Mixer motor stopped.");
}

bool startMixers() {
  digitalWrite(mixerOne, LOW);
  digitalWrite(mixerTwo, LOW);
  mixerStartMillis = millis();
  isMixerRunning = true;
  Serial.println("Mixer started.");
  return true;
}

bool stopMixers() {
  digitalWrite(mixerOne, HIGH);
  digitalWrite(mixerTwo, HIGH);
  isMixerRunning = false;
  Serial.println("Mixer stopped.");
  return true;
}
int cocoaAdding() {
  int choose = 2; 
  char key = keypad.getKey(); 

  if (key) {
    Serial.print("Key pressed: ");
    Serial.println(key);

    if (key == '1') {
      myServo.write(0); 
      delay(2000); 
      myServo.write(90); 
      choose = 0; 
    } else if (key == '2') {
      choose = 1;
    }
  }

  return choose; 
}

