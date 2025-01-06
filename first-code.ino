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

int mixingTimeInMinutes = 1; 
unsigned long mixingStartTime = 0; 
bool isMixing = false;

void mixerUpInterrupt(){
  stopMixerUP = true;
  Serial.println("Mixer Up interrupt triggered.");
}

void mixerDownInterrupt(){
  stopMixerDown = true;
  Serial.println("Mixer Down interrupt triggered.");
}

void forwardInterrupt() {
  stopForward = true;  
  Serial.println("Forward limit switch pressed. Stopping forward movement.");
}

void reverseInterrupt() {
  stopReverse = true;  
  Serial.println("Reverse limit switch pressed. Stopping reverse movement.");
}

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
  digitalWrite(mixerDown ,  HIGH);  
  digitalWrite(mixerUp,HIGH);
}

void startMixers() {
  Serial.println("Mixers started.");
  digitalWrite(mixerOne , LOW);  
  digitalWrite(mixerTwo, LOW);
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

  attachInterrupt(digitalPinToInterrupt(railwaylimitSwitchForward), forwardInterrupt, FALLING); 
  attachInterrupt(digitalPinToInterrupt(railwaylimitSwitchReverse), reverseInterrupt, FALLING); 

  pinMode(mixerlimitSwitchUp, INPUT_PULLUP); 
  pinMode(mixerlimitSwitchDown, INPUT_PULLUP); 
  pinMode(mixerOne, OUTPUT);          
  pinMode(mixerTwo, OUTPUT); 
  pinMode(mixerUp, OUTPUT);          
  pinMode(mixerDown, OUTPUT); 

  attachInterrupt(digitalPinToInterrupt(mixerlimitSwitchUp), mixerUpInterrupt, FALLING); 
  attachInterrupt(digitalPinToInterrupt(mixerlimitSwitchDown), mixerDownInterrupt, FALLING); 

  Serial.println("System setup complete.");
}
void loop() {

if (stopForward){
  reverseFunction();
  stopForward=false;
}
  if (stopReverse && systemStart) {
    stopReverse = false; 
    stopRailwayMovement(); 
    mixerMoveDown();
  }

  if (!isMixing && stopMixerDown) {
    stopMixersMotor();
    stopMixerDown = false; 
    mixingStartTime = millis(); 
    isMixing = true;
    startMixers();
   
  }

  if (isMixing && millis() - mixingStartTime >= mixingTimeInMinutes * 60000) { 
    stopMixers(); 
    mixerMoveUp(); 
    isMixing = false;
   
  }


  if (stopMixerUP) {
    stopMixerUP = false;  
    stopMixersMotor();
    forwardFunction(); 
    
  }
}