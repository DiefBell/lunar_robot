#define NUM_SENSORS 2
#define ULONG_MAX 4294967295

const float MM_PER_US = 0.343;

const int pTrigger[NUM_SENSORS] = {0, 1};
const int pEcho[NUM_SENSORS] = {2, 3};
unsigned long echoTimer[NUM_SENSORS] = {0, 0};
unsigned int echoDuration[NUM_SENSORS] = {0, 0};

enum sensors : int
{
  FL = 0,
  FR = 1
};

unsigned long prevTest = 0;
const int TEST_INTERVAL = 500;

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
  initSensor<0>(); initSensor<1>();
}

void loop()
{
  if(millis() > prevTest + TEST_INTERVAL)
  {
    Serial.print("Sensor 0: ");  Serial.print(echoDuration[0] * MM_PER_US); Serial.println(" mm.");
    Serial.print("Sensor 1: ");  Serial.print(echoDuration[1] * MM_PER_US); Serial.println(" mm.");
    
    Trigger(0);
    Trigger(1);
    prevTest = millis();
  }
}

template <int SENSOR_NUM> void initSensor()
{
  attachInterrupt(digitalPinToInterrupt(pEcho[SENSOR_NUM]), GetEchoTime<SENSOR_NUM>, RISING);
}

template <int SENSOR_NUM> void GetEchoTime()
{
  unsigned long currTime = micros();  // get time as close to interrupt as possible
  if (echoTimer[SENSOR_NUM] > currTime) echoDuration[SENSOR_NUM] = (ULONG_MAX - echoTimer[SENSOR_NUM]) + currTime; // if micros() has overflowed
  else echoDuration[SENSOR_NUM] = currTime - echoTimer[SENSOR_NUM];
  Serial.print("Echo time ["); Serial.print(SENSOR_NUM); Serial.print("]: "); Serial.print(echoDuration[SENSOR_NUM]); Serial.println(" us.");
}

void Trigger(int pin)
{
  digitalWrite(pTrigger[pin], HIGH);
  delayMicroseconds(10);
  digitalWrite(pTrigger[pin], LOW);
}
