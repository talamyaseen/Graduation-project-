

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
  mixerStartMillis = millis();  // Store the time when mixers start
  isMixerRunning = true;
}

void stopMixers() {
  Serial.println("Mixers stopped.");
  digitalWrite(mixerOne , HIGH);  
  digitalWrite(mixerTwo, HIGH);
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

  digitalWrite(mixerOne, HIGH); 
  digitalWrite(mixerTwo, HIGH);
}

void loop() {

 
  // Check for limit switch presses
 if (digitalRead(railwaylimitSwitchForward) == LOW) {  // Forward switch pressed
    Serial.println("Forward limit switch pressed. Stopping forward movement.");
    reverseFunction();  // Call reverse function when forward button is pressed
  }

  if (digitalRead(railwaylimitSwitchReverse) == LOW) {  // Reverse switch pressed
    Serial.println("Reverse limit switch pressed. Stopping reverse movement.");
    forwardFunction();
    delay(1000);
    stopRailwayMovement();
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
