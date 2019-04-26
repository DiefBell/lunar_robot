#include <Wire.h>
#define ADDR_SLAVE 0x60

#define DEBUG

/*--- MAIN FUNCTIONS ---*/
void setup()
{
  /* I2C setup */
  Wire.begin(ADDR_SLAVE);  // join i2c bus with address 0x60
  Wire.onReceive(eReceive);  // run function "eReceive" when receiving bytes over I2C
  Wire.onRequest(eSend);  // run function "eSend" when bytes are requested

  /* Serial debugging */
#ifdef DEBUG
  Serial.begin(115200);
  Serial.println("Starting...");
#endif
}

void loop()
{

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

void eReceive(int numBytes)
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

}

void eSend()
{

}


/*--- MOTOR FUNCTIONS ---*/
#define MICROSTEP_SETTING 32
void setMotorSpeeds(float rpm)
{

}
/*--- ULTRASONIC FUNCTIONS ---*/

/*--- UTILITY FUNCTION ---*/
