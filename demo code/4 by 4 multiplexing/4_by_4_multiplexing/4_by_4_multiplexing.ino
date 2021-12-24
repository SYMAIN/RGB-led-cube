////LED TEST 2 w/ 74HC595
////by Amanda Ghassaei 2012
////https://www.instructables.com/id/Multiplexing-with-Arduino-and-the-74HC595/
//
///*
// * This program is free software; you can redistribute it and/or modify
// * it under the terms of the GNU General Public License as published by
// * the Free Software Foundation; either version 2 of the License, or
// * (at your option) any later version.
// *
// * This program is distributed in the hope that it will be useful,
// * but WITHOUT ANY WARRANTY; without even the implied warranty of
// * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// * GNU General Public License for more details.
//*/
//
////this code sends data to the 74HC595 without "shiftOut"
//
////pin connections- the #define tag will replace all instances of "latchPin" in your code with A1 (and so on)
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
//
////storage for led states, 4 bytes
//byte ledData[] = {1, 3, 7, 15};
//
//void setup() {
//  //set pins as output
//  pinMode(latchPin, OUTPUT);
//  pinMode(clockPin, OUTPUT);
//  pinMode(dataPin, OUTPUT);
//}
//
//void loop() {
//  
//  for (i=0;i<4;i++){
//    
//    //send data from ledData to each row, one at a time
//    byte dataToSend = (1 << (i+4)) | (15 & ~ledData[i]);
//      
//    // setlatch pin low so the LEDs don't change while sending in bits
//    digitalWrite(latchPin, LOW);
//    
////    // shift out the bits of dataToSend to the 74HC595
////    shiftOut(dataPin, clockPin, LSBFIRST, dataToSend);
//// the code below is the equivalent of the two lines above
//    for (j=0;j<8;j++){
//      digitalWrite(clockPin,LOW);
//      digitalWrite(dataPin,((dataToSend>>j)&1));
//      digitalWrite(clockPin,HIGH);
//    }
//
//
//    //set latch pin high- this sends data to outputs so the LEDs will light up
//    digitalWrite(latchPin, HIGH);
//      
//  }  
//}
//


//LED TEST w/ 74HC595
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

//this code will light up each led in the 4x4 matrix one at a time


//pin connections- the #define tag will replace all instances of "latchPin" in your code with A1 (and so on)
#define latchPin 11
#define clockPin 9
#define dataPin 12

//looping variables
byte i;
byte j;

//storage variable
byte dataToSend;

void setup() {
  //set pins as output
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
}

void loop() {
  testdimming();
}

void testdimming() {
  int counter = 0;
  for (i=0;i<4;i++){
    for (j=0;j<4;j++){
      dataToSend = (1 << (i+4)) | (15 & ~(1 << j));
      if (counter++ % 2 == 1) {
        sendData(00000000);
      }else{
        sendData(dataToSend); 
      }
      delay(0);
    }
  }
  
}

void sendData(int d) {
  // setlatch pin low so the LEDs don't change while sending in bits
  digitalWrite(latchPin, LOW);
  // shift out the bits of dataToSend to the 74HC595
  shiftOut(dataPin, clockPin, LSBFIRST, d);
  //set latch pin high- this sends data to outputs so the LEDs will light up
  digitalWrite(latchPin, HIGH);
}


void allOn(){
    for (i=0;i<4;i++){
      
      for (j=0;j<4;j++){
        
        //bit manipulation (more info at http://arduino.cc/en/Reference/Bitshift ,  http://arduino.cc/en/Reference/BitwiseXorNot , and http://arduino.cc/en/Reference/BitwiseAnd)
        dataToSend = (1 << (i+4)) | (15 & ~(1 << j));//preprare byte (series of 8 bits) to send to 74HC595
        //for example when i =2, j = 3,
        //dataToSend = (1 << 6) | (15 & ~(1 << 3));
        //dataToSend = 01000000 | (15 & ~(00001000));
        //dataToSend = 01000000 | (15 & 11110111);
        //dataToSend = 01000000 | (15 & 11110111);
        //dataToSend = 01000000 | 00000111;
        //dataToSend = 01000111;
        //the first four bits of dataToSend go to the four rows (anodes) of the LED matrix- only one is set high and the rest are set to ground
        //the last four bits of dataToSend go to the four columns (cathodes) of the LED matrix- only one is set to ground and the rest are high
        //this means that going through i = 0 to 3 and j = 0 to three with light up each led once
        
      sendData(dataToSend);
        
        delay(0);//wait
      }
  }
}
