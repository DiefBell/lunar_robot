#include <NewPing.h>
#include <Tone.h>
#include <Servo.h>

const int RPM = 20;
const double GEARBOX_RATIO = 99 + (1044.0 / 2057);
const float STEP_ANGLE = 1.8 / GEARBOX_RATIO;
const int MICROSTEP_SETTING = 32;
uint32_t _StepFreq(float rpm, double stepAngle, int microstep) { return rpm * 6.0 * microstep / stepAngle; }

Tone motorLeft;
const int pinStepLeft = 5;
const int pinDirLeft = 4;
Tone motorRight;
const int pinStepRight = 6;
const int pinDirRight = 7;

Servo carousel;
const int pinCarousel = 9;
Servo collector;
const int pinCollector = 10;

void setup()
{
  delay(1000);
  Serial.begin(115200);
  delay(500);
  Serial.print("Step frequency: "); Serial.println(_StepFreq(RPM, STEP_ANGLE, MICROSTEP_SETTING) );
  delay(500);
  pinMode(pinDirLeft, OUTPUT);
  digitalWrite(pinDirLeft, LOW);
  pinMode(pinDirRight, OUTPUT);
  digitalWrite(pinDirRight, LOW);
  
  motorLeft.begin(pinStepLeft);
  motorLeft.play(_StepFreq(RPM, STEP_ANGLE, MICROSTEP_SETTING) );
  
  motorRight.begin(pinStepRight);
  motorRight.play(_StepFreq(RPM, STEP_ANGLE, MICROSTEP_SETTING) );
  
  carousel.attach(pinCarousel);
  collector.attach(pinCollector);
}

void loop()
{

}
