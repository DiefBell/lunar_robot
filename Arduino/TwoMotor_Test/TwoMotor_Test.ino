const int PIN_DIR_LEFT = 4;
const int PIN_STEP_LEFT = 5;
const int PIN_DIR_RIGHT = 7;
const int PIN_STEP_RIGHT = 6;

const double GEARBOX_RATIO = 99 + (1044.0 / 2057);
const float STEP_ANGLE = 1.8 / GEARBOX_RATIO;

const double MICROSTEP_RATIO = 1.0 / 32;
const float RPM = 7;
int usPerHalfStep = MICROSTEP_RATIO * STEP_ANGLE * 60 * 1000000 / (2.0 * RPM * 360);

#define DIR LOW
 
void setup()
{
  pinMode(PIN_DIR_LEFT, OUTPUT);
  pinMode(PIN_STEP_LEFT, OUTPUT);
  digitalWrite(PIN_DIR_LEFT, !DIR);

  pinMode(PIN_DIR_RIGHT, OUTPUT);
  pinMode(PIN_STEP_RIGHT, OUTPUT);
  digitalWrite(PIN_DIR_RIGHT, DIR);

  Serial.begin(115200);
  Serial.print("Microseconds per half step: "); Serial.println(usPerHalfStep);
  Serial.print("Steps per second: "); Serial.println(1000000 / usPerHalfStep);
}

void loop()
{
  digitalWrite(PIN_STEP_LEFT, LOW);
  digitalWrite(PIN_STEP_RIGHT, LOW);
  delayMicroseconds(usPerHalfStep);
  digitalWrite(PIN_STEP_LEFT, HIGH);
  digitalWrite(PIN_STEP_RIGHT, HIGH);
  delayMicroseconds(usPerHalfStep);
}
