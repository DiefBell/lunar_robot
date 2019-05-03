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

#define REG_DIR_LEFT DDRE  // direction register for port E
#define REG_PIN_LEFT PINE  // input port E
#define REG_OUT_LEFT PORTE  // output port E
#define BIT_STEP_LEFT PE3 // third bit in port E
#define P_DIR_LEFT 4

#define REG_DIR_RIGHT DDRH  // direction register for port H
#define REG_PIN_RIGHT PINH // input port E
#define REG_OUT_RIGHT PORTH  // output port H
#define BIT_STEP_RIGHT PH3  // third bit in port H
#define P_DIR_RIGHT 7

#define MICROSTEP_LOCO 32
const double GEARBOX_RATIO = 99 + (1044.0 / 2057);
const double STEP_ANGLE_LOCO = 1.8 / GEARBOX_RATIO;
#define RPM_LOCO 20

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
  OCR3A = usPerStep(STEP_ANGLE_LOCO, MICROSTEP_LOCO, RPM_LOCO) / US_PER_COUNT;

  // Enable the interrupts
  TIMSK3 = _BV(OCIE3A);

  Serial.begin(115200);
  Serial.print("US_PER_COUNT = "); Serial.println(US_PER_COUNT);
  Serial.print("Microseconds per locomotion step: "); Serial.println(2.0*usPerStep(STEP_ANGLE_LOCO, MICROSTEP_LOCO, RPM_LOCO));
  Serial.print("\tOCR3A: "); Serial.println(OCR3A);

  // motor directions
  pinMode(P_DIR_LEFT, OUTPUT);
  pinMode(P_DIR_RIGHT, OUTPUT);
  digitalWrite(P_DIR_LEFT, LOW);
  digitalWrite(P_DIR_RIGHT, HIGH);

  // configure step pins to outputs
  REG_DIR_LEFT |= _BV(BIT_STEP_LEFT);
  REG_DIR_RIGHT |= _BV(BIT_STEP_RIGHT);
  
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
  REG_OUT_LEFT ^= _BV(BIT_STEP_LEFT); REG_OUT_RIGHT ^= _BV(BIT_STEP_RIGHT);
  //digitalWrite(5, !digitalRead(5) ); digitalWrite(6, !digitalRead(6) );
  //PORTE ^= _BV(PE3); PORTH ^= _BV(PH3);
}
