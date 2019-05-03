#define PRESCALER 1
#define CLOCK_SPEED_MHZ 16.0
const double US_PER_COUNT = PRESCALER/CLOCK_SPEED_MHZ;
const uint32_t US_PER_MINUTE = 60000000;

float usPerStep(float stepAngle, int microstep, float rpm)
{
  uint32_t t = stepAngle * US_PER_MINUTE;  // need a bigger variable to handle the US_PER_MINUTE value
  float us = t / (microstep * rpm * 360 * 2.0);  // the two exists because it needs to be an on/off per step
  return us;
}

/*** MOTOR Y ***
#define REG_DIR DDRE  // direction register for port E
#define REG_PIN PINE  // input port E
#define REG_OUT PORTE  // output port E
#define REG_BIT PE3 // third bit in port E
#define P_DIR 4
#define P_STEP 5
*/

/*** MOTOR X ***/
#define REG_DIR DDRH  // direction register for port H
#define REG_PIN PINH // input port E
#define REG_OUT PORTH  // output port H
#define REG_BIT PH3  // third bit in port H
#define P_DIR 7
#define P_STEP 6


// LOCOMOTION MOTORS
//const double GEARBOX_RATIO = 99 + (1044.0 / 2057);
//#define RAW_STEP_ANGLE 1.8

// LEAD SCREW MOTOR
const double GEARBOX_RATIO = 1.0;
#define RAW_STEP_ANGLE 0.9

const double STEP_ANGLE = RAW_STEP_ANGLE / GEARBOX_RATIO;
#define RPM 10
#define MICROSTEP 32
#define DIR LOW

long halfStepTime;

void setup()
{
  cli();
  
  // set prescaler to 1, 1 timer count is 62.5ns
  TCCR3B &= ~_BV(CS32);  // 0
  TCCR3B &= ~_BV(CS31);  // 0
  TCCR3B |= _BV(CS30);  // 1

  // reset timer3 on compare
  TCCR3A &= ~_BV(WGM30); // 0
  TCCR3A &= ~_BV(WGM31); // 0
  TCCR3B |= _BV(WGM32);  // 1
  TCCR3B &= ~_BV(WGM33); // 0

  // set values for the timer interrupts
  OCR3A = usPerStep(STEP_ANGLE, MICROSTEP, RPM) / US_PER_COUNT;

  // Enable the interrupts
  TIMSK3 = _BV(OCIE3A);

  Serial.begin(115200);
  //Serial.print("US_PER_COUNT = "); Serial.println(US_PER_COUNT);
  Serial.print("Microseconds per step: "); Serial.println(2.0*usPerStep(STEP_ANGLE, MICROSTEP, RPM));
  Serial.print("\tOCR3A: "); Serial.println(OCR3A);

  // motor directions
  pinMode(P_DIR, OUTPUT);
  digitalWrite(P_DIR, DIR);

  // configure step pins to outputs
  //REG_DIR |= _BV(REG_BIT);
  pinMode(P_STEP, OUTPUT);
  digitalWrite(P_STEP, LOW);
  
  // reset timer 3 to zero
  TCNT3 = 0;

  halfStepTime = usPerStep(STEP_ANGLE, MICROSTEP, RPM);

  // enable global interrupts
  sei();
}

void loop()
{/*
  digitalWrite(P_STEP, HIGH);
  delayMicroseconds(halfStepTime);
  digitalWrite(P_STEP, LOW);
  delayMicroseconds(halfStepTime);*/
}

ISR(TIMER3_COMPA_vect)  // interrupt for the movement steppers
{
  REG_OUT ^= _BV(REG_BIT);
  //digitalWrite(P_STEP, !digitalRead(P_STEP) );
  //PORTE ^= _BV(PE3);
}
