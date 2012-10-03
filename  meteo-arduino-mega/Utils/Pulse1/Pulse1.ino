int pin = 8;
unsigned long duration;

void setup()
{
  Serial.begin(9600);
  pinMode(pin, INPUT_PULLUP);
}

void loop()
{
  duration = pulseIn(pin, LOW, 3000000);
  Serial.print("Pulse length ");
  Serial.print(duration);
  Serial.println(" ms");
}
