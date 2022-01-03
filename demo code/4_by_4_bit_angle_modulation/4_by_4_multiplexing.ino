// The Carry
// https://www.kevindarrah.com/download/8x8x8/RGB_CubeV12_BitwiseFix.ino

#include <SPI.h>// SPI Library used to clock data out to the shift registers

//pin connections- the #define tag will replace all instances of "latchPin" in your code with A1 (and so on)
#define latchPin 11
#define clockPin 51 // The SPI-MOSI pinout on the Arduino Mega. Varies between Arduinos.
#define dataPin 52 // The SPI-SCK pinout on the Arduino Mega. Varies between Arduinos.

//looping variables
byte i;
byte j;

// core of BAM, 4 bit resolution
byte led0[2], led1[2], led2[2], led3[2];

void setup() {

  SPI.setBitOrder(MSBFIRST);//Most Significant Bit First 

  // what are those
  SPI.setDataMode(SPI_MODE0);// Mode 0 Rising edge of data, keep clock low
  SPI.setClockDivider(SPI_CLOCK_DIV2);//Run the data in at 16MHz/2 - 8MHz
  
  //set pins as output
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {

}