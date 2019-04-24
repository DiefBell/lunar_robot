#define MAX_SENSORS 6
#define NUM_SENSORS 1
#define ULONG_MAX 4294967295

const double CM_PER_US = 343 * 100.0 / 1000000.0;

const int pTrigger[MAX_SENSORS] = {0, 1, 8, 9, 16, 17};
const int pEcho[MAX_SENSORS] = {21, 20, 2, 3, 18, 19};
volatile unsigned long echoTimer[MAX_SENSORS] = {0, 0, 0, 0, 0, 0};
volatile unsigned int echoDuration[MAX_SENSORS] = {0, 0, 0, 0, 0, 0};

enum sensors : int
{
  FL = 0,
  FR = 1,
  LEFT = 2,
  RIGHT = 3,
  BL = 4,
  BR = 5
};

unsigned long prevTest = 0;
const int TEST_INTERVAL = 2000;

void setup()
{
  Serial.begin(115200);
  Serial.print(FL);
  for (int i = 0; i < NUM_SENSORS; i++)
  {
    pinMode(pTrigger[i], OUTPUT);
    digitalWrite(pTrigger[i], LOW);
    pinMode(pEcho[i], INPUT);
  }
  initSensor<FL>(); //initSensor<FR>(); initSensor<LEFT>(); initSensor<RIGHT>(); //initSensor<BL>(); initSensor<BR>();
}

void loop()
{
  if (millis() > prevTest + TEST_INTERVAL)
  {
    for(int i = 0; i < NUM_SENSORS; i++)
    {
      Serial.print("Sensor "); Serial.print(i); Serial.print(": ");  Serial.print(echoDuration[0] * CM_PER_US / 2); Serial.println(" cm.");
    }

    Trigger(FL);
    //Trigger(FR);
    //Trigger(LEFT);
    //Trigger(RIGHT);
    //Trigger(BL);
    //Trigger(BR);

    prevTest = millis();
  }
}

template <int SENSOR_NUM> void initSensor()
{
  attachInterrupt(digitalPinToInterrupt(pEcho[SENSOR_NUM]), GetEchoTime<SENSOR_NUM>, CHANGE);
  delay(10);
}


template <int SENSOR_NUM> void GetEchoTime()
{
  if (digitalRead(pEcho[SENSOR_NUM]) == LOW) {
    unsigned long currTime = micros();  // get time as close to interrupt as possible
    if (echoTimer[SENSOR_NUM] > currTime) echoDuration[SENSOR_NUM] = (ULONG_MAX - echoTimer[SENSOR_NUM]) + currTime; // if micros() has overflowed
    else echoDuration[SENSOR_NUM] = currTime - echoTimer[SENSOR_NUM];  }
  else{
    echoTimer[SENSOR_NUM] = micros();
  }
}

/* Doesn't seem to need this, seems to be happy just self-triggering */
void Trigger(int sensorNum)
{
  digitalWrite(pTrigger[sensorNum], HIGH);
  delayMicroseconds(10);
  digitalWrite(pTrigger[sensorNum], LOW);
}
