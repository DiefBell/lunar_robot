#include <Servo.h>

#define PIN_S_CAROUSEL A0
Servo carousel;

#define NUM_POTS 5
#define STEP_ANGLE 24
#define NEUTRAL_ANGLE 15

void setup()
{
  carousel.attach(PIN_S_CAROUSEL);
  carousel.write(NEUTRAL_ANGLE);
}

void loop()
{
  carousel.write(NEUTRAL_ANGLE);
  delay(2000);
  carousel.write(NEUTRAL_ANGLE + (2*STEP_ANGLE) );
  delay(1000);
  for(int i = 1; i < NUM_POTS; i++)
  {
    carousel.write(carousel.read()+ STEP_ANGLE);
    delay(1000);
  }

}
