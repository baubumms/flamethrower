//#define SERIAL_OUT

#define TRIGGER_PIN 2
#define ARC_PIN 4
#define VALVE_PIN 6

#define MAX_THROWING_TIME 5000 //max duration in ms
#define ARC_IGNITION_TIME 500

bool state = false;
bool lastState = false;

bool arcing = false;
bool firing = false;

long fireTime = 0;
long arcTime = 0;

void setup() {
  #ifdef SERIAL_OUT
  Serial.begin(9600);
  #endif
  
  pinMode(TRIGGER_PIN, INPUT_PULLUP);
  groundPin(3);
  pinMode(ARC_PIN, OUTPUT);
  pinMode(VALVE_PIN, OUTPUT);
  groundPin(5);

  attachInterrupt(digitalPinToInterrupt(TRIGGER_PIN), triggerChange, CHANGE);
}

void loop() {
  checkState();
  checkTimers();
}

void checkState(){
  //was the trigger triggered?
  if(state == lastState)
    return;

  if(!state)
    stopFire();
  else{
    delay(3);
    if(digitalRead(TRIGGER_PIN) == HIGH){
      state = false;
      return;
    }
    startFire();
  }
    

  lastState = state;
}

void checkTimers(){
  //asynchronosly turn of fire extinguishing after delay
  if(millis() - fireTime >= MAX_THROWING_TIME && state && firing)
    stopFire();

  //asynchronosly turn of arc iginition extinguishing after delay
  if( millis() - arcTime >= ARC_IGNITION_TIME && state && arcing)
    stopArc();
}

void triggerChange(){
  if(digitalRead(TRIGGER_PIN) == LOW)
    state = true;
  else
    state = false;
}

void startFire(){
  digitalWrite(ARC_PIN, HIGH);
  digitalWrite(VALVE_PIN, HIGH);
  
  fireTime = millis();
  arcTime = millis();
  arcing = true;
  firing = true;

  #ifdef SERIAL_OUT
  Serial.println("fire started");
  #endif
}

void stopFire(){
  digitalWrite(VALVE_PIN, LOW);
  digitalWrite(ARC_PIN, LOW);//if the igintion timeout wasnt reached
  arcing = false;
  firing = false;
  
  #ifdef SERIAL_OUT
  Serial.println("fire stopped");
  #endif
}

void stopArc(){
  digitalWrite(ARC_PIN, LOW);
  arcing = false;

  #ifdef SERIAL_OUT
  Serial.println("arc stopped");
  #endif
}

void groundPin(int pin){
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
}
