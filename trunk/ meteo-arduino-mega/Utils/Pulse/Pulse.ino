volatile unsigned int Ticks;	   // holds the pulse count as .5 us ticks
int icpPin = 8;			     // this interrupt handler must use pin 8


ISR(TIMER1_CAPT_vect){
  Serial.println("CAPT");
   if( bit_is_set(TCCR1B ,ICES1)){	 // was rising edge detected ?
	 TCNT1 = 0;				// reset the counter
   }
   else {					  // falling edge was detected
	  Ticks = ICR1;
   }
   TCCR1B ^= _BV(ICES1);		     // toggle bit value to trigger on the other edge
}

void setup()			  // run once, when the sketch starts
{
  Serial.begin(9600);
  pinMode(icpPin,INPUT_PULLUP);
  TCCR1A = 0x00;	   // COM1A1=0, COM1A0=0 => Disconnect Pin OC1 from Timer/Counter 1 -- PWM11=0,PWM10=0 => PWM Operation disabled
  TCCR1B = 0x02;	   // 16MHz clock with prescaler means TCNT1 increments every .5 uS (cs11 bit set
  Ticks = 0;		 // default value indicating no pulse detected
  TIMSK1 = _BV(ICIE1);   // enable input capture interrupt for timer 1
}

int getTick() {
  int akaTick;	 // holds a copy of the tick count so we can return it after re-enabling interrupts
  cli();		 //disable interrupts
  akaTick = Ticks;
  sei();		 // enable interrupts
  return akaTick;
}

void loop()			   // run over and over again
{
  static int prevTick = 0;

  if( getTick()  != prevTick) {
     prevTick = getTick();
     Serial.print( prevTick);     // print the tick value only when it changes
  }
} 

