#include <NewPing.h>
#include <Tone.h>
#include <Servo.h>
#include <Wire.h>

#define Stepper Tone
#define move play
#define Ultrasonic NewPing

#define _StepFreq(x,y) (x*360)/(y*60)
class StepperMotor
{
  public:
    StepperMotor(int pStep, int _pDir, float _stepAngle)
    {
      pDir = _pDir;
      pinMode(pDir, OUTPUT);
      digitalWrite(pDir, LOW);
      
      stepAngle = _stepAngle;
      stepper.begin(pStep);
    }
    void run(int _rpm)
    {
      rpm = _rpm;
      digitalWrite(pDir, rpm > 0);  // set direction
      stepper.move(_StepFreq(abs(rpm), stepAngle) );
    }
    void stop()  // add something for sleeping?
    {
      rpm = 0;
      stepper.stop();
    }
    float rpm = 0;  // Suspect that checking equality with this isn't going to work
  private:
    int pDir;
    float stepAngle;
    Stepper stepper;
};

/* COMMUNICATION */
#define addrArduino 0x60
int receiveBuffer[5] = {0, 0, 0, 0, 0};

/* LOCOMOTION */
const double GEARBOX_RATIO = 99 + (1044.0 / 2057);
StepperMotor LeftMotor(5, 4, 1.8 / GEARBOX_RATIO);
StepperMotor RightMotor(6, 7, 1.8 / GEARBOX_RATIO);

/* DRILLING */
StepperMotor DrillMotor(12, 13, 0.9);

/* ULTRASONIC */
/*
const int pTrigger[6] = {0, 2, 8, 10, A0, A2};
const int pEcho[6] = {1, 3, 9, 11, A1, A3};
Ultrasonic sensors[6] =
{
  Ultrasonic(pTrigger[0], pTrigger[0]),
  Ultrasonic(pTrigger[1], pTrigger[1]),
  Ultrasonic(pTrigger[2], pTrigger[2]),
  Ultrasonic(pTrigger[3], pTrigger[3]),
  Ultrasonic(pTrigger[4], pTrigger[4]),
  Ultrasonic(pTrigger[5], pTrigger[5])
};
int sensorValues[6] = {0, 0, 0, 0, 0, 0};
const unsigned int pingInterval = 100;  // time between pings in ms
unsigned long pingTimer;  // records when the next ping should occur
const unsigned int maxDistance = 500;*/

void setup()
{
  Wire.begin(addrArduino);  // join i2c bus with address 0x60
  Wire.onReceive(eOnReceive);
  Serial.begin(115200);
}

void loop()
{
  
}

void eOnReceive(int numBytes)
{
  if (numBytes > 5) return;
  int count = 0;
  while(Wire.available() ) receiveBuffer[count++] = Wire.read();

  if(receiveBuffer[0] == 0)  // movement
  {
    if(LeftMotor.rpm != receiveBuffer[1]) LeftMotor.run(receiveBuffer[1]);
    if(RightMotor.rpm != receiveBuffer[2]) RightMotor.run(receiveBuffer[2]);
    if(DrillMotor.rpm != 0) DrillMotor.stop();
   
  }
  else  // drilling
  {
    if(LeftMotor.rpm != 0) LeftMotor.stop();
    if(RightMotor.rpm != 0) RightMotor.stop();
    if(DrillMotor.rpm != receiveBuffer[1]) DrillMotor.run(receiveBuffer[1]);
  }
}
