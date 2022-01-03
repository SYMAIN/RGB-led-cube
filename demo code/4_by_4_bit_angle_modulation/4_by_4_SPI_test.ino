unsigned char defeatTheCrumbyPreprocessor;
// https://forum.arduino.cc/t/tccr1a-was-not-declared-in-this-scope/177826/4

// The Carry
// https://www.kevindarrah.com/download/8x8x8/RGB_CubeV12_BitwiseFix.ino
// https://www.youtube.com/watch?v=xmScytz9y0M

#include <SPI.h>// SPI Library used to clock data out to the shift registers

//pin connections- the #define tag will replace all instances of "latchPin" in your code with A1 (and so on)
#define latchPin 11
#define clockPin 51 // The SPI-MOSI pinout on the Arduino Mega. Varies between Arduinos.
#define dataPin 52 // The SPI-SCK pinout on the Arduino Mega. Varies between Arduinos.

void setup() {

  SPI.setBitOrder(MSBFIRST);//Most Significant Bit First 

  // what are those
  SPI.setDataMode(SPI_MODE0);// Mode 0 Rising edge of data, keep clock low
  SPI.setClockDivider(SPI_CLOCK_DIV2);//Run the data in at 16MHz/2 - 8MHz
  
  noInterrupts();

  // register spaghetti (hope it works on a different Arduino circuit!)
  TCCR1A = B00000000;//Register A all 0's since we're not toggling any pins
  TCCR1B = B00001011;//bit 3 set to place in CTC mode, will call an interrupt on a counter match
  //bits 0 and 1 are set to divide the clock by 64, so 16MHz/64=250kHz
  TIMSK1 = B00000010;//bit 1 set to call the interrupt on an OCR1A match
  OCR1A=30;

  //set pins as output
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  
  Serial.begin(9600);
  SPI.begin();//start up the SPI library
  interrupts();//let the show begin, this lets the multiplexing start

}

void loop() {
}


// gee golly hope this works!!!!!! (relies on registers)
// The BAM timer.
ISR(TIMER1_COMPA_vect){//***MultiPlex BAM***MultiPlex BAM***MultiPlex BAM***MultiPlex BAM***MultiPlex BAM***MultiPlex BAM***MultiPlex BAM

    Serial.print("Sending!");
    SPI.transfer(0b11100001);

    PORTD |= 1<<latchPin;//Latch pin HIGH
    PORTD &= ~(1<<latchPin);//Latch pin LOW

}