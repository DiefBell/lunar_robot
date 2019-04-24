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

#define RD_STEP_LEFT DDRE  // data direction register E
#define RP_STEP_LEFT PORTE  // port E
#define RB_STEP_LEFT PE3 // third bit
#define P_DIR_LEFT 4
#define RD_STEP_RIGHT DDRH  // data direction register H
#define RP_STEP_RIGHT PORTH  // port H
#define RB_STEP_RIGHT PH3  // third bit
#define P_DIR_RIGHT 7
#define MICROSTEP_LOCO 16
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
  cli();
  
  // set prescaler to 8, 1 timer count is 0.5us
  TCCR3B &= ~_BV(CS32);  // 0
  TCCR3B &= ~_BV(CS31);  // 0
  TCCR3B |= _BV(CS30);  // 1

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

  // motor directions
  pinMode(P_DIR_LEFT, OUTPUT);
  pinMode(P_DIR_RIGHT, OUTPUT);
  digitalWrite(P_DIR_LEFT, LOW);
  digitalWrite(P_DIR_RIGHT, HIGH);

  // configure step pins to outputs
  RD_STEP_LEFT |= _BV(RB_STEP_LEFT);
  RD_STEP_RIGHT |= _BV(RB_STEP_RIGHT);
  
  // reset timer 3 to zero
  TCNT3 = 0;

  // enable global interrupts
  sei();
}

void loop()
{
  while(1);
}

ISR(TIMER3_COMPA_vect)  // interrupt for the movement steppers
{
  digitalWrite(5, !digitalRead(5) ); digitalWrite(6, !digitalRead(6) );
  //PORTE ^= _BV(PE3); PORTH ^= _BV(PH3);
  
  TCNT3 = 0;  // reset timer3
}

ISR(TIMER3_COMPB_vect)  // interrupt for the drilling stepper
{
  digitalWrite(P_STEP_DRILL, !digitalRead(P_STEP_DRILL) );
  TCNT3 = 0;  // reset timer3
}
