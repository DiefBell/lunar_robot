void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}const int PIN_DIR_LEFT = 4;
const int PIN_STEP_LEFT = 5;
const int PIN_DIR_RIGHT = 7;
const int PIN_STEP_RIGHT = 6;

const double GEARBOX_RATIO = 99 + (1044.0 / 2057);
const float STEP_ANGLE = 1.8 / GEARBOX_RATIO;

const double MICROSTEP_RATIO = 1.0 / 32;
const float RPM = 34;
int usPerStep = MICROSTEP_RATIO * STEP_ANGLE * 60 * 1000000 / (RPM * 360);

void setup()
{
  pinMode(PIN_DIR_LEFT, OUTPUT);
  pinMode(PIN_STEP_LEFT, OUTPUT);
  digitalWrite(PIN_DIR_LEFT, LOW);

  pinMode(PIN_DIR_RIGHT, OUTPUT);
  pinMode(PIN_STEP_RIGHT, OUTPUT);
  digitalWrite(PIN_DIR_RIGHT, LOW);

  Serial.begin(115200);
  Serial.print("Microseconds per step: "); Serial.println(usPerStep);
  Serial.print("Steps per second: "); Serial.println(1000000 / usPerStep);
}

void loop()
{
  digitalWrite(PIN_STEP_LEFT, LOW);
  digitalWrite(PIN_STEP_RIGHT, LOW);
  delayMicroseconds(usPerStep / 2.0);
  digitalWrite(PIN_STEP_LEFT, HIGH);
  digitalWrite(PIN_STEP_RIGHT, HIGH);
  delayMicroseconds(usPerStep / 2.0);
}
