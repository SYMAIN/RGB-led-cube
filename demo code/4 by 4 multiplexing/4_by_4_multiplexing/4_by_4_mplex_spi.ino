//
//#define latchPin 11
//#define clockPin 9
//#define dataPin 12
//
////looping variables
//byte i;
//byte j;
//
////storage variable
//byte dataToSend;
//byte red0[64], red1[64], red2[64], red3[64];
//byte blue0[64], blue1[64], blue2[64], blue3[64];
//byte green0[64], green1[64], green2[64], green3[64];
//
//void setup() {
//  //set pins as output
//  pinMode(latchPin, OUTPUT);
//  pinMode(clockPin, OUTPUT);
//  pinMode(dataPin, OUTPUT);
//}
//
//void loop() {
//  test();
//}
//
//void test() {
//  sendData(B10000111);
//  delay(500);
//  
//  sendData(B10001011);
//  delay(5);
//  sendData(B01000111);
//  delay(5);
//}
//
//void sendData(int d) {
//  // setlatch pin low so the LEDs don't change while sending in bits
//  digitalWrite(latchPin, LOW);
//  // shift out the bits of dataToSend to the 74HC595
//  shiftOut(dataPin, clockPin, LSBFIRST, d);
//  //set latch pin high- this sends data to outputs so the LEDs will light up
//  digitalWrite(latchPin, HIGH);
//}
//
//
//void allOn(){
//    for (i=0;i<4;i++){
//      
//      for (j=0;j<4;j++){
//        
//        //bit manipulation (more info at http://arduino.cc/en/Reference/Bitshift ,  http://arduino.cc/en/Reference/BitwiseXorNot , and http://arduino.cc/en/Reference/BitwiseAnd)
//        dataToSend = (1 << (i+4)) | (15 & ~(1 << j));//preprare byte (series of 8 bits) to send to 74HC595
//        //for example when i =2, j = 3,
//        //dataToSend = (1 << 6) | (15 & ~(1 << 3));
//        //dataToSend = 01000000 | (15 & ~(00001000));
//        //dataToSend = 01000000 | (15 & 11110111);
//        //dataToSend = 01000000 | (15 & 11110111);
//        //dataToSend = 01000000 | 00000111;
//        //dataToSend = 01000111;
//        //the first four bits of dataToSend go to the four rows (anodes) of the LED matrix- only one is set high and the rest are set to ground
//        //the last four bits of dataToSend go to the four columns (cathodes) of the LED matrix- only one is set to ground and the rest are high
//        //this means that going through i = 0 to 3 and j = 0 to three with light up each led once
//        
//        sendData(dataToSend);
//        
//        delay(0);//wait
//      }
//  }
//}


//LED TEST 2 w/ 74HC595
//by Amanda Ghassaei 2012
//https://www.instructables.com/id/Multiplexing-with-Arduino-and-the-74HC595/

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
*/

//this code sends data to the 74HC595 without "shiftOut"

// BLANK: Yellow, pin 8
// LATCH: Yellow, pin 11
// DATA: Orange, pin 51
// CLOCK: green, pin 52

#include <SPI.h>// SPI Library used to clock data out to the shift registers

//pin connections- the #define tag will replace all instances of "latchPin" in your code with A1 (and so on)
#define latchPin 11 // aka PB5
#define blankPin 8
#define latchPinBIN 0b00100000 // this is the internal representation of turning PORTB5 on, aka the latch pin.
#define clockPin 52 // The SPI-MOSI pinout on the Arduino Mega. Varies between Arduinos.
#define dataPin 51 // The SPI-SCK pinout on the Arduino Mega. Varies between Arduinos.

void setup() {
  //set pins as output
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  Serial.begin(9600);

  noInterrupts();

  // register spaghetti from tutorial (hope it works on a different Arduino circuit!)
  TCCR1A = B00000000;//Register A all 0's since we're not toggling any pins
  TCCR1B = B00001011;//bit 3 set to place in CTC mode, will call an interrupt on a counter match
  //bits 0 and 1 are set to divide the clock by 64, so 16MHz/64=250kHz
  TIMSK1 = B00000010;//bit 1 set to call the interrupt on an OCR1A match
  OCR1A=30;

  SPI.setBitOrder(MSBFIRST);//Most Significant Bit First 
  // what are those
  SPI.setDataMode(SPI_MODE0);// Mode 0 Rising edge of data, keep clock low
  SPI.setClockDivider(SPI_CLOCK_DIV2);//Run the data in at 16MHz/2 - 8MHz
  SPI.begin();//start up the SPI library

  interrupts();//let the show begin, this lets the multiplexing start
}

void loop() {
}

ISR(TIMER1_COMPA_vect){
  PORTB &= ~(latchPinBIN);//Latch pin LOW
  SPI.transfer(0b11100001);
  PORTB |= latchPinBIN;//Latch pin HIGH

  PORTB &= ~(latchPinBIN);//Latch pin LOW
  SPI.transfer(0b11100010);
  PORTB |= latchPinBIN;//Latch pin HIGH

//   PORTB &= ~(latchPinBIN);//Latch pin LOW
//   SPI.transfer(0b11100100);
//   PORTB |= latchPinBIN;//Latch pin HIGH

//   PORTB &= ~(latchPinBIN);//Latch pin LOW
//   SPI.transfer(0b11100100);
//   PORTB |= latchPinBIN;//Latch pin HIGH
}

void sendData(int d) {
  // setlatch pin low so the LEDs don't change while sending in bits
  //digitalWrite(latchPin, LOW);
  PORTB &= ~(latchPinBIN);//Latch pin LOW
  // shift out the bits of dataToSend to the 74HC595
  shiftOut(dataPin, clockPin, LSBFIRST, d);
  //set latch pin high- this sends dataa to outputs so the LEDs will light up
  //digitalWrite(latchPin, HIGH);
  PORTB |= latchPinBIN;//Latch pin HIGH
}