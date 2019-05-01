#define PRESCALER 1
#define CLOCK_SPEED_MHZ 16.0
const float US_PER_COUNT = PRESCALER/CLOCK_SPEED_MHZ;
#define US_PER_MINUTE 60000000

uint16_t usPerStep(float stepAngle, int microstep, float rpm)
{
  uint32_t us = stepAngle * US_PER_MINUTE;  // need a bigger variable to handle the US_PER_MINUTE value
  us /= (microstep * rpm * 360 * 2);  // the two exists because it needs to be an on/off per step
  return (uint16_t)us;
}

#define P_STEP_LEFT 5
#define P_DIR_LEFT 4
#define P_STEP_RIGHT 6
#define P_DIR_RIGHT 7
#define MICROSTEP_LOCO 32
const float GEARBOX_RATIO = 99 + (1044.0 / 2057);
const float STEP_ANGLE_LOCO = 1.8 / GEARBOX_RATIO;
#define RPM_LOCO 20

#define P_STEP_DRILL 8
#define P_DIR_DRILL 9
#define MICROSTEP_DRILL 32
const float STEP_ANGLE_DRILL = 0.9;
#define RPM_DRILL 20

void setup()
{
  // set prescaler to 8, 1 timer count is 0.5us
  TCCR3B &= ~_BV(CS32);
  TCCR3B &= ~_BV(CS31);
  TCCR3B |= _BV(CS30);

  // set values for the timer interrupts
  OCR3A = usPerStep(STEP_ANGLE_LOCO, MICROSTEP_LOCO, RPM_LOCO) / US_PER_COUNT;
  OCR3B = usPerStep(STEP_ANGLE_DRILL, MICROSTEP_DRILL, RPM_DRILL) / US_PER_COUNT;

  // Enable the interrupts
  TIMSK3 = _BV(OCIE3A);
  //TIMSK3 |= _BV(OCIE3B);

  Serial.begin(115200);
  Serial.print("US_PER_COUNT = "); Serial.println(US_PER_COUNT);
  Serial.print("Microseconds per locomotion step: "); Serial.println(usPerStep(STEP_ANGLE_LOCO, MICROSTEP_LOCO, RPM_LOCO));
  Serial.print("\tOCR3A: "); Serial.println(OCR3A);
  Serial.print("Microseconds per drill step: "); Serial.println(usPerStep(STEP_ANGLE_DRILL, MICROSTEP_DRILL, RPM_DRILL));
  Serial.print("\tOCR3B: "); Serial.println(OCR3B);

  pinMode(P_DIR_LEFT, OUTPUT); digitalWrite(P_DIR_LEFT, LOW);
  pinMode(P_DIR_RIGHT, OUTPUT); digitalWrite(P_DIR_RIGHT, HIGH);

  TCNT3 = 0; // reset timer 3 to zero
}

void loop()
{

}

ISR(TIMER3_COMPA_vect)  // interrupt for the movement steppers
{
  digitalWrite(P_STEP_LEFT, !digitalRead(P_STEP_LEFT) );
  digitalWrite(P_STEP_RIGHT, !digitalRead(P_STEP_RIGHT) );
  TCNT3 = 0;  // reset timer3
}

ISR(TIMER3_COMPB_vect)  // interrupt for the drilling stepper
{
  digitalWrite(P_STEP_DRILL, !digitalRead(P_STEP_DRILL) );
  TCNT3 = 0;  // reset timer3
}
