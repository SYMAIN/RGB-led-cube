#define LATCH_pin 11      // (11) ST_CP [RCK] on 74HC595
#define CLCOK_pin  9      // (9) SH_CP [SCK] on 74HC595
#define DATA_pin 12     // (12) DS [S1] on 74HC595
int tdelay = 100;

void clock_signal(void){
   digitalWrite(CLCOK_pin, HIGH);
    delayMicroseconds(500);
   digitalWrite(CLCOK_pin, LOW);
    delayMicroseconds(500);
}
void latch_enable(void)
   {
    digitalWrite(LATCH_pin, HIGH);
    delayMicroseconds(500);
    digitalWrite(LATCH_pin, LOW);
    }
void send_data(unsigned int data_out)
{
    int i;
    unsigned hold;
    for (i=0 ; i<8 ; i++)
    {
        if ((data_out >> i) & (0x01))
        digitalWrite(DATA_pin,HIGH); 
        else
        digitalWrite(DATA_pin,LOW); 
        
        clock_signal();
    }
    latch_enable(); // Data finally submitted
}

void setup() 
{
  pinMode(LATCH_pin , OUTPUT);
  pinMode(DATA_pin , OUTPUT);  
  pinMode(CLCOK_pin , OUTPUT);
  digitalWrite(LATCH_pin, LOW);      // (11) ST_CP [RCK] on 74HC595
  digitalWrite(CLCOK_pin, LOW);      // (9) SH_CP [SCK] on 74HC595
  digitalWrite(DATA_pin, LOW);     // (12) DS [S1] on 74HC595
  Serial.begin(9600);
}

void loop() 
{
  effect1();
}

void effect1() {
  for (int i=0;i<8;i++) {
    byte leds = 1 << i;
    send_data(leds);
    delay(tdelay);
  }
  byte mbyte = B10000000;
  for (int i=0;i<8;i++) {
    byte leds = mbyte >> i;
    send_data(leds);
    delay(tdelay);
  }
}
