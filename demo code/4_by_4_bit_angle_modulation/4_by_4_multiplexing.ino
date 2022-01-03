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

//These variables are used by multiplexing and Bit Angle Modulation Code
int shift_out;//used in the code a lot in for(i= type loops

int level=0;//keeps track of which level we are shifting data to
int anodelevel=0;//this increments through the anode levels
int BAM_Bit, BAM_Counter=0; // Bit Angle Modulation variables to keep track of things

// the data to send inside the BAM counter
byte BAMDataToSend;

// core of BAM, 4 bit resolution. Each LED is represented 4 times (aka 4-bit res).
// the 4 most significant bits are ignored in each byte
byte led0[4], led1[4], led2[4], led3[4];

// Since this is a 4x4 instead of an 8x8x8, this is scaled down.
// as a result, the 4 most significant bits are ignored in each byte
byte anode[4];

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

  anode[0]=0b00000001;
  anode[1]=0b00000010;
  anode[2]=0b00000100;
  anode[3]=0b00001000;

  //set pins as output
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  
  Serial.begin(9600);
  SPI.begin();//start up the SPI library
  interrupts();//let the show begin, this lets the multiplexing start

}

// Using interrupts so no need to put anything here.
void loop() {

}

// Heavily simplified LED function from the video.

/** void LED(int column, byte brightness)
  * 
  * @param row, int. The index of the LED row. Supports 0-3. For getting a byte from the byte array. (aka led0, led1, led2, led3)
  * @param column, int. The index of the led in the row to light up. Supports 0-3. For accessing a bit in a byte.
  * @param brightness, byte. Controls bit angle modulation. Only the 4 least significant bits are used.
  *
  */
void LED(int row, int column, byte brightness){

  // writes a bit to led0: (row, col, boolean: turn on or off depending on BAM)
  bitWrite(led0[row], column, bitRead(brightness, 0));
  bitWrite(led1[row], column, bitRead(brightness, 1));
  bitWrite(led2[row], column, bitRead(brightness, 2)); 
  bitWrite(led3[row], column, bitRead(brightness, 3)); 

}

// gee golly hope this works!!!!!! (relies on registers)
// The BAM timer.
ISR(TIMER1_COMPA_vect){//***MultiPlex BAM***MultiPlex BAM***MultiPlex BAM***MultiPlex BAM***MultiPlex BAM***MultiPlex BAM***MultiPlex BAM

  // sanity check
  // Serial.print("yo this is actually working bruhhhhhhhhh\n");

  // for calculating the brightness using BAM
  // you can think of BAM_Bit as the brightness for BAM: on for 1 tick, off for 2, on for 4, on for 8, etc....
  // no idea why the numbers are scuffed
  if(BAM_Counter==8)
  BAM_Bit++;
  else
  if(BAM_Counter==24)
  BAM_Bit++;
  else
  if(BAM_Counter==56)
  BAM_Bit++;

  BAM_Counter++;//Here is where we increment the BAM counter

// Here, we have to do some bitmath to send out both anode + cathode information all at once since
// we need to send out 1 byte for SPI.transfer() to work. Since anode info is a total of 4 bits and cathode is
// a total of 4 bits as well, we can combine both pieces of information to create a byte.
// We wouldn't need to do this if we had an 8x8, but we dont so yea
switch (BAM_Bit){
case 0:
 // level here refers to the z-axis. Since we dont have one, treat it as 0!
 for(shift_out=level; shift_out<level+8; shift_out++)
 BAMDataToSend = 
 SPI.transfer(BAMDataToSend);
 for(shift_out=level; shift_out<level+8; shift_out++)
 BAMDataToSend = 
 SPI.transfer(BAMDataToSend);
 for(shift_out=level; shift_out<level+8; shift_out++)
 BAMDataToSend = 
 SPI.transfer(BAMDataToSend);
  break;
case 1:
 for(shift_out=level; shift_out<level+8; shift_out++)
 SPI.transfer(red1[shift_out]);
 for(shift_out=level; shift_out<level+8; shift_out++)
 SPI.transfer(green1[shift_out]); 
 for(shift_out=level; shift_out<level+8; shift_out++)
 SPI.transfer(blue1[shift_out]);
  break;
 case 2:
 for(shift_out=level; shift_out<level+8; shift_out++)
 SPI.transfer(red2[shift_out]);
 for(shift_out=level; shift_out<level+8; shift_out++)
 SPI.transfer(green2[shift_out]); 
 for(shift_out=level; shift_out<level+8; shift_out++)
 SPI.transfer(blue2[shift_out]);
 break;
 case 3:
 for(shift_out=level; shift_out<level+8; shift_out++)
 SPI.transfer(red3[shift_out]);
 for(shift_out=level; shift_out<level+8; shift_out++)
 SPI.transfer(green3[shift_out]); 
 for(shift_out=level; shift_out<level+8; shift_out++)
 SPI.transfer(blue3[shift_out]);
 //Here is where the BAM_Counter is reset back to 0, it's only 4 bit, but since each cycle takes 8 counts,
 //, it goes 0 8 16 32, and when BAM_counter hits 64 we reset the BAM
  if(BAM_Counter==120){
  BAM_Counter=0;
  BAM_Bit=0;
  }
  break;
}//switch_case

// we would send out anode info here if we had an 8x8x8
// SPI.transfer(anode[anodelevel]);//finally, send out the anode level byte

PORTD |= 1<<latch_pin;//Latch pin HIGH
PORTD &= ~(1<<latch_pin);//Latch pin LOW



}