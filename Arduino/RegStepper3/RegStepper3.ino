struct Pin {
  int pin;
  uint8_t bitMask;
  uint8_t port;
  uint8_t* modeRegister;
  uint8_t* outputRegister;
};

Pin initPin(int pinNum) {
  Pin p;
  p.pin = pinNum;
  p.bitMask = digitalPinToBitMask(p.pin);
  p.port = digitalPinToPort(p.pin);
  p.modeRegister = portModeRegister(p.port);
  p.outputRegister = portOutputRegister(p.port);
  return p;
}

#define PRESCALER 1
#define CLOCK_SPEED_MHZ 16.0
#define US_PER_MINUTE 60000000
uint16_t usPerStep(float stepAngle, int microstep, float rpm) {
  uint32_t us = stepAngle * US_PER_MINUTE;  // need a bigger variable to handle the US_PER_MINUTE value
  us /= (microstep * rpm * 360 * 2);  // the two exists because it needs to be an on/off per step
  return (uint16_t)us;
}

#define MICROSTEP_LOCO 8
const float GEARBOX_RATIO = 99 + (1044.0 / 2057);
const float STEP_ANGLE_LOCO = 1.8 / GEARBOX_RATIO;
#define RPM_LOCO 5

volatile Pin DIR_LEFT = initPin(4);
volatile Pin STEP_LEFT = initPin(5);
volatile Pin STEP_RIGHT = initPin(6);
volatile Pin DIR_RIGHT = initPin(7);

void setup()
{
  /* Disable interrupts */
  cli();

  // set timer3's prescaler to 1, 1 timer count is 1/16th of a microsecond
  TCCR3B &= ~_BV(CS32);  // 0
  TCCR3B &= ~_BV(CS31);  // 0
  TCCR3B |= _BV(CS30);  // 1

  // set value for timer3's compare interrupt
  OCR3A = usPerStep(STEP_ANGLE_LOCO, MICROSTEP_LOCO, RPM_LOCO) * PRESCALER / CLOCK_SPEED_MHZ;

  // enable timer3 compareA interrupt
  TIMSK3 = _BV(OCIE3A);
  
  /* Setup motor locomotion pins */
  *STEP_LEFT.modeRegister |= STEP_LEFT.bitMask;  // set to outputs
  *STEP_RIGHT.modeRegister |= STEP_RIGHT.bitMask;
  *DIR_LEFT.modeRegister |= DIR_LEFT.bitMask;
  *DIR_RIGHT.modeRegister |= DIR_RIGHT.bitMask;
  *DIR_LEFT.outputRegister |= DIR_LEFT.bitMask;  // set HIGH
  *DIR_RIGHT.outputRegister &= ~DIR_RIGHT.bitMask;  // set LOW

  /* Re-enable interrupts */
  sei();

  /* Some debug info */
  Serial.begin(115200);
  Serial.println(usPerStep(STEP_ANGLE_LOCO, MICROSTEP_LOCO, RPM_LOCO));
}

void loop()
{
  //
}

ISR(TIMER3_COMPA_vect)
{
  *STEP_LEFT.outputRegister ^= STEP_LEFT.bitMask;
  *STEP_RIGHT.outputRegister ^= STEP_RIGHT.bitMask;

  TCNT3 = 0;  // reset timer3
}
