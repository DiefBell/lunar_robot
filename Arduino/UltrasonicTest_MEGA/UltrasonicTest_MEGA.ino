#define NUM_SENSORS 6
#define ULONG_MAX 4294967295

const int pTrigger[NUM_SENSORS] = {0, 1, 14, 15, 16, 17};
const int pEcho[NUM_SENSORS] = {2, 3, 18, 19, 20, 21};
unsigned long echoTimer[NUM_SENSORS] = {0, 0, 0, 0, 0, 0};
int echoDuration[NUM_SENSORS] = {0, 0, 0, 0, 0, 0};

void setup()
{
  for(int i = 0; i < NUM_SENSORS; i++)
  {
    pinMode(pTrigger[i], OUTPUT);
    digitalWrite(pTrigger[i], LOW);
    pinMode(pEcho[i], INPUT);
    attachInterrupt(digitalPinToInterrupt(pEcho[i]), checkSensors, RISING);
    echoTimer[i] = micros();
  }
}

void loop()
{

}

void checkSensors()
{
  unsigned long currTime = micros();
  for (int i = 0; i < NUM_SENSORS; i++)
  {
    if(digitalRead(pEcho[i]) )
    {
      if(currTime < echoTimer[i]) echoDuration[i] = (ULONG_MAX - echoTimer[i]) + currTime;  // micros timer has overflown
      else echoDuration[i] = currTime - echoTimer[i];

      digitalWrite(pTrigger[i], HIGH);
      delayMicroseconds(10);
      digitalWrite(pTrigger[i], LOW);
    }
  }
}
