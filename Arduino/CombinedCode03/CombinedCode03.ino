#include <Wire.h>  // for I2C functionality

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
#define USRF_FL 0
#define USRF_FR 1
#define USRF_BL 2
#define USRF_BR 3

bool usrfStop = false;
unsigned long t_prevTrigger = 0;
uint16_t usrf_timer[4] = {0, 0, 0, 0};
uint16_t usrf_dur[4] = {0, 0, 0, 0};
int usrf_dist[4] = {0, 0, 0, 0};

/***** I2C *****/
//I2C
byte ib[8] = {0, 0, 0, 0, 0, 0, 0, 0};  // i2c buffer
unsigned long t_prevUpdate = 0;

/***** TIMERS *****/
#define US_PER_MINUTE 60000000
#define CLOCK_SPEED_MHZ 16.0
#define PRESCALER_3 1
const double US_PER_COUNT_3 PRESCALER_3 / CLOCK_SPEED_MHZ;
#define PRESCALER_4 64
const double US_PER_COUNT_4 PRESCALER_4 / CLOCK_SPEED_MHZ;


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


/***** MAIN FUNCTIONS *****/

void setup()
{

}

void loop()
{
  usrfStop = (ib[B_USRF_FL] > usrf_dist[USRF_FL] && ib[B_DIR_L] == 1) ||  // if the front left sensor is under its limit and the left legs are moving forward
     (ib[B_USRF_FR] > usrf_dist[USRF_FR] && ib[B_DIR_R] == 1) ||
     (ib[B_USRF_BL] > usrf_dist[USRF_BL] && ib[B_DIR_L] == 0) ||
     (ib[B_USRF_BR] > usrf_dist[USRF_BR] && ib[B_DIR_R] == 0) )
     
  if(ib[B_MODE] != 1 || ib[B_RPM] == 0 || usrfStop)   
  {
    stopMotors();
  }
}

void stopMotors()
{
  TIMSK3 &= ^_BV(OCIE3A);  // turn off the interrupt
}
void startMotors(int rpm)
{
  uint32_t us = US_PER_MINUTE * STEP_ANGLE;  // need a 32bit variable to store this
  us /= rpm * MICROSTEP_RATIO * 360.0;  // microsends per step
  
  #ifdef DEBUG
  Serial.print("Microseconds per step: "); Serial.print(us); Serial.println("us");
  #endif

  OCR3A = us / (US_PER_COUNT * 2);  // number of timer3 counts before the CompA interrupt is triggered
  TIMSK3 - _BV(OCIE3A);  // enable the interrupt
}
ISR(TIMER3_COMPA_vect)  // interrupt for the movement steppers
{
  digitalWrite(PIN_STEP_LEFT, !digitalRead(PIN_STEP_LEFT) ); digitalWrite(PIN_STEP_RIGHT, !digitalRead(PIN_STEP_RIGHT) );
  //PORTE ^= _BV(PE3); PORTH ^= _BV(PH3);
  
  TCNT3 = 0;  // reset timer3 (eventually set this to CTC mode in setup() )
}
