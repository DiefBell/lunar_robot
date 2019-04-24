#define PIN_TRIG_FL 11
#define PIN_ECHO_FL 2

#define PIN_TRIG_FR 9
#define PIN_ECHO_FR 19

#define PIN_TRIG_R 13
#define PIN_ECHO_R 3

#define US2CM(x) 0.343*x/2

long timeOfLastTrig = 0;
const int INTERVAL = 200;

long timer;
long duration;
int distance;

void setup()
{
  Serial.begin(115200);
  Serial.println("Starting...");
  
  pinMode(PIN_TRIG_FL, OUTPUT);
  pinMode(PIN_ECHO_FL, INPUT);
  digitalWrite(PIN_TRIG_FL, LOW);

  /*
  pinMode(PIN_TRIG_FR, OUTPUT);
  pinMode(PIN_ECHO_FR, INPUT);
  digitalWrite(PIN_TRIG_FR, LOW);
  */
  
  attachInterrupt(digitalPinToInterrupt(PIN_ECHO_FL), timeEcho<PIN_ECHO_FL>, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(PIN_ECHO_FR), timeEcho<PIN_ECHO_FR>, CHANGE);
}

void loop()
{
  if(millis() - timeOfLastTrig > INTERVAL)
  {
    digitalWrite(PIN_TRIG_FL, HIGH);
    //digitalWrite(PIN_TRIG_FR, HIGH);
    delayMicroseconds(10);
    digitalWrite(PIN_TRIG_FL, LOW);
    //digitalWrite(PIN_TRIG_FR, LOW);
    timeOfLastTrig = millis();
  }
}

template<int PIN>
void timeEcho()
{
  if(digitalRead(PIN) == HIGH) {
    timer = micros();
  }else{
    duration = micros() - timer;
    distance = US2CM(duration);
    Serial.print("-- Echo on pin ("); Serial.print(PIN); Serial.println(")--");
    Serial.print("\tDuration: "); Serial.print(duration); Serial.println("us.");
    Serial.print("\tDistance: "); Serial.print(distance); Serial.println("mm.\n");
  }
}
