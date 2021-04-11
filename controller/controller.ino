//#define SERIAL_OUT

#define TRIGGER_PIN 2
#define ARC_PIN 4
#define VALVE_PIN 6

#define MAX_THROWING_TIME 5000 //max duration in ms
//#define ARC_IGNITION_TIME 500

//triigger states
bool state = false;
bool lastState = false;

bool arcing = false;
bool firing = false;

//timestamps of individual actions to stop asyncly after timeout
long fireTime = 0;
long arcTime = 0;

void setup() {
  #ifdef SERIAL_OUT
  Serial.begin(9600);
  #endif
  
  pinMode(TRIGGER_PIN, INPUT_PULLUP);
  groundPin(3);//ground for trigger button (didn't want to solder to many wires to one gnd pin -> remove if you can solder)
  pinMode(ARC_PIN, OUTPUT);
  pinMode(VALVE_PIN, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(TRIGGER_PIN), triggerChanged, CHANGE);//call callback every time the trigger gets activated
}

void loop() {
  checkState();
  checkTimers();
}

void checkState(){//react if state changed
  //trigger triggered?
  if(state == lastState)
    return;

  if(!state)
    stopFire();
  else{
    //filter out weird behaviour when trigger button is in transition between triggered and not triggered state
    delay(3);
    if(digitalRead(TRIGGER_PIN) == HIGH)
      state = false;
    else
      startFire();
  }

  lastState = state;
}

void checkTimers(){//react if time limits reached
  //asynchronosly turn of fire extinguishing after delay
  if(millis() - fireTime >= MAX_THROWING_TIME && state && firing)
    stopFire();

#ifdef ARC_IGNITION_TIME 
  //asynchronosly turn of arc iginition after delay
  if( millis() - arcTime >= ARC_IGNITION_TIME && state && arcing)
    stopArc();
#endif
}

void triggerChanged(){//callback for interrupt on change of trigger 
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

void groundPin(int pin){//connects pin to ground - only capable for dealing with low currents
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
}
