int pin = A3;
unsigned long duration;

void setup() {
  Serial.begin(9600);
  pinMode(pin, INPUT);
}

void loop()
{
  duration = pulseIn(pin, HIGH);
  Serial.println(duration);
  //delay(500);
}

