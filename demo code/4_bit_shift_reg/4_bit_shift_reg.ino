// C++ code
//
int tdelay = 500;
byte leds = 0;

//Pin connected to ST_CP of 74HC595
int latchpin = 8;
//Pin connected to SH_CP of 74HC595
int clockpin = 12;
////Pin connected to DS of 74HC595
int datapin = 11;

void setup()
{
  pinMode(latchpin, OUTPUT);
  pinMode(latchpin, OUTPUT);
  pinMode(latchpin, OUTPUT);
}

void updateShiftRegistor() {
  digitalWrite(latchpin, LOW);
  shiftOut(datapin, clockpin,LSBFIRST,leds);
  digitalWrite(latchpin,HIGH);
}  

void loop()
{
  for (int i=0;i<4;i++) {
    leds = 1 << i;
    updateShiftRegistor();
    delay(tdelay);
  }
  
   for (int i=0;i<4;i++) {
     byte mybyte = B1000;
    leds = mybyte >> i;
    updateShiftRegistor();
    delay(tdelay);
  }
}
