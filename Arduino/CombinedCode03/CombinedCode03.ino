#include <Wire.h>  // for I2C functionality
#include <Servo.h>  // for servos (obviously)

/***** I2C BUFFER POSITIONS *****/
//mode, 0=off, 1=move, 2=drill
#define B_MODE 0
//motor bytes
#define B_RPM 1
#define B_DIR_R 2
#define B_DIR_L 3
//USRF limits
#define B_USRF_FL 4
#define B_USRF_FR 5
#define B_USRF_BL 6
#define B_USRF_BR 7
//servos
#define B_SERVO_CAR 2  // byte for the carousel position
#define B_SERVO_COL 3  // and for the collector

/***** ULTRASONIC RANGE FINDERS *****/
#define NUM_SENSORS 4
#define US2CM(x) 0.343*x/2

#define USRF_FL 0
#define USRF_FR 1
#define USRF_BL 2
#define USRF_BR 3

const int PIN_TRIG[4] = {11, 9, 13, 10};
const int PIN_ECHO[4] = {2, 19, 3, 18};

bool usrfStop = false;
unsigned long t_prevTrigger = 0;
const int INTERVAL_TRIGGER = 100;  // 100ms
uint16_t usrf_timer[4] = {0, 0, 0, 0};
uint16_t usrf_dur[4] = {0, 0, 0, 0};
int usrf_dist[4] = {0, 0, 0, 0};

/***** I2C *****/
//I2C
#define ADDR_SLAVE 0x60
byte ib[8] = {0, 0, 0, 0, 0, 0, 0, 0};  // i2c buffer
unsigned long t_prevUpdate = 0;
const int INTERVAL_UPDATE = 500;  // maximum of half a second between updates, else motors stop

/***** TIMERS *****/
#define US_PER_MINUTE 60000000
#define CLOCK_SPEED_MHZ 16.0
#define PRESCALER_3 1
const double US_PER_COUNT_3 = PRESCALER_3 / CLOCK_SPEED_MHZ;
#define PRESCALER_4 64
const double US_PER_COUNT_4 = PRESCALER_4 / CLOCK_SPEED_MHZ;


/***** STEPPER MOTORS *****/
#define MICROSTEP_SETTING 32
const double GEARBOX_RATIO = 99 + (1044.0/2057);
const double MICROSTEP_RATIO = 1.0 / MICROSTEP_SETTING;
const double STEP_ANGLE = 1.8 / GEARBOX_RATIO;

#define PIN_STEP_LEFT 5
#define PIN_DIR_LEFT 4
#define PIN_STEP_RIGHT 6
#define PIN_DIR_RIGHT 7

/***** SERVOS *****/
Servo sCarousel;
Servo sCollector;

#define PIN_S_CAR A0
#define PIN_S_COL A5

/***** MAIN FUNCTIONS *****/

void setup()
{
  // disable interrupts during setup
  cli();

  // start i2C and enable receive and request events
  Wire.begin(ADDR_SLAVE);
  Wire.onReceive(eReceive);
  Wire.onRequest(eRequest);
  
  // set prescaler to 1 for timer3 (one timer count per count of the main clock)
  TCCR3B |= _BV(CS30);  // 1
  TCCR3B &= ~_BV(CS31); // 0
  TCCR3B &= ~_BV(CS32); // 0

  // reset timer3 on compare
  TCCR3A &= ~_BV(WGM30); // 0
  TCCR3A &= ~_BV(WGM31); // 0
  TCCR3B |= _BV(WGM32);  // 1
  TCCR3B &= ~_BV(WGM33); // 0

  // set prescaler to 64 for timer4 (one timer count per 64 counts of the main clock)
  TCCR4B |= _BV(CS40);  // 1
  TCCR4B |= _BV(CS41);  // 1
  TCCR4B &= ~_BV(CS42); // 0

  // setup pins for motors
  pinMode(PIN_STEP_LEFT, OUTPUT); digitalWrite(PIN_STEP_LEFT, LOW);
  pinMode(PIN_STEP_RIGHT, OUTPUT); digitalWrite(PIN_STEP_RIGHT, LOW);
  pinMode(PIN_DIR_LEFT, OUTPUT); digitalWrite(PIN_DIR_LEFT, LOW);
  pinMode(PIN_DIR_RIGHT, OUTPUT); digitalWrite(PIN_DIR_RIGHT, HIGH);
  
  // set pin modes for USRF pins
  for(int i = 0; i < NUM_SENSORS; i++)
  {
    pinMode(PIN_TRIG[i], OUTPUT);
    pinMode(PIN_ECHO[i], INPUT);
    digitalWrite(PIN_TRIG[i], LOW);
  }
  
  // attach interrupts to the USRFs to time length of the echo pulse
  // can't be done in a for loop due to use of templates
  attachInterrupt(digitalPinToInterrupt(PIN_ECHO[USRF_FL] ), timeEcho<USRF_FL>, CHANGE); 
  attachInterrupt(digitalPinToInterrupt(PIN_ECHO[USRF_FR] ), timeEcho<USRF_FR>, CHANGE); 
  attachInterrupt(digitalPinToInterrupt(PIN_ECHO[USRF_BL] ), timeEcho<USRF_BL>, CHANGE); 
  attachInterrupt(digitalPinToInterrupt(PIN_ECHO[USRF_BR] ), timeEcho<USRF_BR>, CHANGE); 

  // attach servos
  sCarousel.attach(PIN_S_CAR);
  sCollector.attach(PIN_S_COL);
  sCarousel.write(90);  // test position
  sCollector.write(90);  // test position
  
  // set up serial debugging
  #ifdef DEBUG
  Serial.begin(115200);
  Serial.println("Starting Horntail...");
  #endif

  // reset timers to zero
  TCNT3 = 0;
  TCNT4 = 0;
  
  // re-enable global interrupts
  sei();
}

void loop()
{
  usrfStop = (ib[B_USRF_FL] > usrf_dist[USRF_FL] && ib[B_DIR_L] == 1) ||  // if the front left sensor is under its limit and the left legs are moving forward
     (ib[B_USRF_FR] > usrf_dist[USRF_FR] && ib[B_DIR_R] == 1) ||
     (ib[B_USRF_BL] > usrf_dist[USRF_BL] && ib[B_DIR_L] == 0) ||
     (ib[B_USRF_BR] > usrf_dist[USRF_BR] && ib[B_DIR_R] == 0);
     
  if(ib[B_MODE] != 1 || ib[B_RPM] == 0 || usrfStop || t_prevUpdate - millis() > INTERVAL_UPDATE)   
  {
    stopMotors();
  }

  if(t_prevTrigger - millis() > INTERVAL_TRIGGER) triggerSensors();
}

/***** I2C INTERRUPTS *****/
void eReceive(int numBytes)
{
  if(numBytes != 8) return;  // command is wrong length!
  int count = 0;
  bool change = false;
  #ifdef DEBUG
    Serial.print("\n[Receiving Command]\nNumber of bytes received: "); Serial.println(numBytes);
  #endif
  
  while (Wire.available() )
  {
    byte b = Wire.read();
    if(b != ib[count])
    {
      #ifdef DEBUG
        Serial.print("  =>"); Serial.println(i2c_buffer[count]);
      #endif
      change = true;
    }
    count++;
  }

  if(change)
  {
    if(ib[B_MODE] == 1 && ib[B_RPM] > 0 )
    {
      digitalWrite(PIN_DIR_LEFT, ib[B_DIR_L] );
      digitalWrite(PIN_DIR_RIGHT, !ib[B_DIR_R] );
      startMotors(ib[B_RPM]);  // currently both motors run at the same speed
    }
  }
}

void eRequest()
{
  // send sensor values
}

/***** USRF FUNCTION *****/
void triggerSensors()
{
  for(int i = 0; i < NUM_SENSORS; i++) digitalWrite(PIN_TRIG[i], HIGH);
  delayMicroseconds(10);
  for(int i = 0; i < NUM_SENSORS; i++) digitalWrite(PIN_TRIG[i], LOW);
  t_prevTrigger = millis();
}

template <int I>
void timeEcho()
{
  if(digitalRead(PIN_ECHO[I]) == HIGH)usrf_timer[I] = TCNT4;
  else
  {
    usrf_dur[I] = US_PER_COUNT_4 * (TCNT4 - usrf_timer[I]);
    usrf_dist[I] = US2CM(usrf_dur[I] );
  }
}

/***** MOTOR FUNCTIONS *****/
void stopMotors()
{
  TIMSK3 &= ~_BV(OCIE3A);  // turn off the interrupt
}
void startMotors(int rpm)
{
  uint32_t us = US_PER_MINUTE * STEP_ANGLE;  // need a 32bit variable to store this
  us /= rpm * MICROSTEP_RATIO * 360.0;  // microsends per step
  
  #ifdef DEBUG
  Serial.print("Microseconds per step: "); Serial.print(us); Serial.println("us");
  #endif

  OCR3A = us / (US_PER_COUNT_3 * 2);  // number of timer3 counts before the CompA interrupt is triggered
  TIMSK3 |= _BV(OCIE3A);  // enable the interrupt
}
ISR(TIMER3_COMPA_vect)  // interrupt for the movement steppers
{
  digitalWrite(PIN_STEP_LEFT, !digitalRead(PIN_STEP_LEFT) ); digitalWrite(PIN_STEP_RIGHT, !digitalRead(PIN_STEP_RIGHT) );
  //PORTE ^= _BV(PE3); PORTH ^= _BV(PH3);  // idk why I can't get this working :/
  }
