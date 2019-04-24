#define pin PB7
#define ddr DDRB
#define port PORTB

void setup()
{
  ddr != _BV(pin);
}

void loop()
{
  port ^= _BV(pin);
  delay(500);
}
