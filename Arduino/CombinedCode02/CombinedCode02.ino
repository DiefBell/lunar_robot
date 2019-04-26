#define DEBUG

#include <Wire.h>
#define ADDR_SLAVE 0x60

#define PIN_DIR_LEFT 4
#define PIN_STEP_LEFT 5
#define PIN_STEP_RIGHT 6
#define PIN_DIR_RIGHT 7

float usPerHalfStep = 0;  // TEMPORARY FOR TESTING

/*--- MAIN FUNCTIONS ---*/
void setup()
{
  /* I2C setup */
  Wire.begin(ADDR_SLAVE);  // join i2c bus with address 0x60
  Wire.onReceive(eReceive);  // run function "eReceive" when receiving bytes over I2C
  Wire.onRequest(eSend);  // run function "eSend" when bytes are requested

  /* Configure stepper pins */
  pinMode(PIN_DIR_LEFT, OUTPUT); digitalWrite(PIN_DIR_LEFT, LOW);
  pinMode(PIN_STEP_LEFT, OUTPUT); digitalWrite(PIN_STEP_LEFT, LOW);
  pinMode(PIN_STEP_RIGHT, OUTPUT); digitalWrite(PIN_STEP_RIGHT, LOW);
  pinMode(PIN_DIR_RIGHT, OUTPUT); digitalWrite(PIN_DIR_RIGHT, LOW);

  /* Serial debugging */
#ifdef DEBUG
  Serial.begin(115200);
  Serial.println("Starting...");
#endif
}

void loop()
{
  if (usPerHalfStep > 0)
  {
    digitalWrite(PIN_STEP_LEFT, LOW);
    digitalWrite(PIN_STEP_RIGHT, LOW);
    delayMicroseconds(usPerHalfStep);
    digitalWrite(PIN_STEP_LEFT, HIGH);
    digitalWrite(PIN_STEP_RIGHT, HIGH);
    delayMicroseconds(usPerHalfStep);
  }
}


/*--- I2C INTERRUPTS ---*/
#define B_MOVE 0  // byte for moving
#define B_RPM_L 1  // rpm of left motor
#define B_RPM_R 2
#define B_DIR_L 3  // direction of left motor
#define B_DIR_R 4
#define B_USRF_FL 5  // byte for the limit of the FL ultrasonic sensor
#define B_USFR_FR 6
#define B_USFR_BL 7
#define B_USFR_BR 8
int i2c_buffer[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

void eReceive(int numBytes)  // need to shrink this down so it runs faster
{
#ifdef DEBUG
  Serial.print("\n[Receiving Command]\nNumber of bytes received: "); Serial.println(numBytes);
#endif
  int count = 0;

  while (Wire.available() )
  {
    i2c_buffer[count] = Wire.read();
#ifdef DEBUG
    Serial.print("  =>"); Serial.println(i2c_buffer[count]);
#endif
    count++;
  }
  if (i2c_buffer[B_RPM_L] > 0 && i2c_buffer[B_RPM_R] > 0 && i2c_buffer[B_MOVE] == 1)
  {
    digitalWrite(PIN_DIR_LEFT, i2c_buffer[B_DIR_L]);
    digitalWrite(PIN_DIR_RIGHT, i2c_buffer[B_DIR_R]);
    setMotorSpeeds(i2c_buffer[B_RPM_L] / 10.0);  // same RPM for both motors atm
#ifdef DEBUG
    Serial.print("Microseconds per step: "); Serial.print(usPerHalfStep * 2.0); Serial.println("us\n");
#endif
  }
  else
  {
    usPerHalfStep = 0;
  }
}

void eSend()
{

}


/*--- MOTOR FUNCTIONS ---*/
#define MICROSTEP_SETTING 32
#define RAW_STEP_ANGLE 1.8
const float MICROSTEP_RATIO = 1.0 / MICROSTEP_SETTING;
const double GEARBOX_RATIO = 99 + (1044.0 / 2057);
const float STEP_ANGLE = RAW_STEP_ANGLE / GEARBOX_RATIO;
const uint64_t usPerMin = 60 * 1000000;

void setMotorSpeeds(float rpm)
{
  usPerHalfStep = MICROSTEP_RATIO * STEP_ANGLE * usPerMin / (rpm * 360.0);
}
/*--- ULTRASONIC FUNCTIONS ---*/

/*--- UTILITY FUNCTION ---*/
