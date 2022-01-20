// This code was made for the Arduino Mega2560.
// Anything related to output registers (PORTxx) and SPI pins will need to be changed if using a different Arduino

unsigned char defeatTheCrumbyPreprocessor;
// https://forum.arduino.cc/t/tccr1a-was-not-declared-in-this-scope/177826/4

// The Carry
// https://www.kevindarrah.com/download/8x8x8/RGB_CubeV12_BitwiseFix.ino
// https://www.youtube.com/watch?v=xmScytz9y0M

#include <SPI.h>// SPI Library used to clock data out to the shift registers

//pin connections- the #define tag will replace all instances of "latchPin" in your code with A1 (and so on)
#define latchPin 11 // aka PB5 / PORTB5. This was found on the schematics of the Arduino Mega2560. 
#define latchPinBIN 0b00100000 // this is the internal representation of turning PORTB5 on, aka the latch pin.
#define blankPin 8 // aka PH5 / PORTH5
#define blankPinBIN 0b00100000 // this is the internal representation of turning PH5 on

#define clockPin 52 // The SPI-MOSI pinout on the Arduino Mega. Varies between Arduinos.
#define dataPin 51 // The SPI-SCK pinout on the Arduino Mega. Varies between Arduinos.

// BLANK: Yellow, pin 8
// LATCH: Yellow, pin 11
// DATA: Orange, pin 51
// CLOCK: green, pin 52

//These variables are used by multiplexing and Bit Angle Modulation Code
int shift_out;//used in the code a lot in for(i= type loops

int level=0;//keeps track of which level we are shifting data to
int anodelevel=0;//this increments through the anode levels
int BAM_Bit, BAM_Counter=0; // Bit Angle Modulation variables to keep track of things

// animation delay
int delayTime = 250;


// the data to send inside the BAM counter
byte BAMDataToSend;

// core of BAM, 4 bit resolution. Each LED is represented 4 times (aka 4-bit res).
// the 4 most significant bits are ignored in each byte
// xxx0 : 1 tick of time
// xxx1 : 2 ticks of time
// xxx2 : 4 ticks of time
// xxx3 : 8 ticks of time
byte red0[8], red1[8], red2[8], red3[8];
byte green0[8], green1[8], green2[8], green3[8];
byte blue0[8], blue1[8], blue2[8], blue3[8];

// Since this is a 4x4 instead of an 8x8x8, this is scaled down.
// as a result, the 4 least significant bits are ignored in each byte
// this is the ground side btw just to avoid confusion
byte anode[4];

void setup() {

  SPI.setBitOrder(MSBFIRST);//Most Significant Bit First 

  // what are those
  SPI.setDataMode(SPI_MODE0);// Mode 0 Rising edge of data, keep clock low
  SPI.setClockDivider(SPI_CLOCK_DIV2);//Run the data in at 16MHz/2 - 8MHz
  
  noInterrupts();

  // register spaghetti from tutorial (hope it works on a different Arduino circuit!)
  TCCR1A = B00000000;//Register A all 0's since we're not toggling any pins
  TCCR1B = B00001011;//bit 3 set to place in CTC mode, will call an interrupt on a counter match
  //bits 0 and 1 are set to divide the clock by 64, so 16MHz/64=250kHz
  TIMSK1 = B00000010;//bit 1 set to call the interrupt on an OCR1A match
  OCR1A=30; // add zeros to this value to slow down the animation. Default: 30

  anode[0]=0b11100000;
  anode[1]=0b11010000;
  anode[2]=0b10110000;
  anode[3]=0b01110000;

  //set pins as output
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(blankPin, OUTPUT);
  PORTH &= ~(blankPinBIN);//Blank pin LOW
  
  Serial.begin(9600);
  SPI.begin();//start up the SPI library
  interrupts();//let the show begin, this lets the multiplexing start

}

bool once = false;

// Put animations here.
void loop() {

  //testPattern();
  // brightnessSwitcherAnimation();

  //nightBAMmer();
  
  //allOn();
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
  // essentially: led_x_[row][col] = brightness[0:3]
  // only writes to the 4 least significant bits

  // ahhh yes, homemade code
  // lol sike https://github.com/anoopmm/RGB_LED_CUBE/blob/master/cubetest.ino
  int whichbyte = int(((level * 16) + (row * 4) + column) / 8);
  int wholebyte = (level * 16) + (row * 4) + column;

  bitWrite(red0[whichbyte], wholebyte - (8 * whichbyte), bitRead(red, 0));
  bitWrite(red1[whichbyte], wholebyte - (8 * whichbyte), bitRead(red, 1));
  bitWrite(red2[whichbyte], wholebyte - (8 * whichbyte), bitRead(red, 2));
  bitWrite(red3[whichbyte], wholebyte - (8 * whichbyte), bitRead(red, 3));

  bitWrite(green0[whichbyte], wholebyte - (8 * whichbyte), bitRead(green, 0));
  bitWrite(green1[whichbyte], wholebyte - (8 * whichbyte), bitRead(green, 1));
  bitWrite(green2[whichbyte], wholebyte - (8 * whichbyte), bitRead(green, 2));
  bitWrite(green3[whichbyte], wholebyte - (8 * whichbyte), bitRead(green, 3));

  bitWrite(blue0[whichbyte], wholebyte - (8 * whichbyte), bitRead(blue, 0));
  bitWrite(blue1[whichbyte], wholebyte - (8 * whichbyte), bitRead(blue, 1));
  bitWrite(blue2[whichbyte], wholebyte - (8 * whichbyte), bitRead(blue, 2));
  bitWrite(blue3[whichbyte], wholebyte - (8 * whichbyte), bitRead(blue, 3));

}


// gee golly hope this works!!!!!! (relies on registers)
// The BAM timer.
ISR(TIMER1_COMPA_vect){

  // Blank everything while we change some stuff
  PORTH |= blankPinBIN;//Blank pin HIGH (aka disable outputs)

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
  if(BAM_Counter==120){
  BAM_Counter=0;
  BAM_Bit=0;
  }

  BAM_Counter++;

  // Here, we have to do some bitmath to send out both anode + cathode information all at once since
  // we need to send out 1 byte for SPI.transfer() to work. Since anode info is a total of 4 bits and cathode is
  // a total of 4 bits as well, we can combine both pieces of information to create a byte.
  // We wouldn't need to do this if we had an 8x8, but we dont so yea
  switch (BAM_Bit){
    case 0:
      BAMDataToSend = ((anode[anodelevel]) | (led0[anodelevel]));
      SPI.transfer(BAMDataToSend);

      // Serial.print("Sent Data ");
      // Serial.print(BAM_Bit);
      // Serial.print(":  ");
      // Serial.print(BAMDataToSend, BIN);
      // Serial.print("\n");

      break;

    case 1:
      BAMDataToSend = ((anode[anodelevel]) | (led1[anodelevel]));
      SPI.transfer(BAMDataToSend);
      Serial.print("\n");
    // Serial.print("Sent Data ");
    // Serial.print(BAM_Bit);
    // Serial.print(":  ");
    // Serial.print(BAMDataToSend, BIN);
    // Serial.print("\n");

      break;

    case 2:
      BAMDataToSend = ((anode[anodelevel]) | (led2[anodelevel]));
      SPI.transfer(BAMDataToSend);

    // Serial.print("Sent Data ");
    // Serial.print(BAM_Bit);
    // Serial.print(":  ");
    // Serial.print(BAMDataToSend, BIN);
    // Serial.print("\n");

      break;

    case 3:
      BAMDataToSend = ((anode[anodelevel]) | (led3[anodelevel]));
      SPI.transfer(BAMDataToSend);

    // Serial.print("Sent Data ");
    // Serial.print(BAM_Bit);
    // Serial.print(":  ");
    // Serial.print(BAMDataToSend, BIN);
    // Serial.print("\n");

      break;
  }

  // better pray for this one to work properly cuz i have no idea whats happening here
  // PORTD |= 1<<latchPin;//Latch pin HIGH
  // PORTD &= ~(1<<latchPin);//Latch pin LOW
  // well looks like praying didnt work because i actually had to change this
  PORTB |= latchPinBIN;//Latch pin HIGH
  PORTB &= ~(latchPinBIN);//Latch pin LOW

  // turn everything back on by disabling blanks
  PORTH &= ~(blankPinBIN);//Latch pin LOW

  anodelevel++;//increment the anode level

  if(anodelevel==4)//go back to 0 if max is reached
    anodelevel=0;

  // Serial.print("Sent Data ");
  // Serial.print(BAM_Bit);
  // Serial.print(":  ");
  // Serial.print(BAMDataToSend, BIN);
  // Serial.print("\n");
}

// gee golly hope this works!!!!!! (relies on registers)
// The BAM timer.
// ISR(TIMER1_COMPA_vect){//***MultiPlex BAM***MultiPlex BAM***MultiPlex BAM***MultiPlex BAM***MultiPlex BAM***MultiPlex BAM***MultiPlex BAM

//   // sanity check
//   // Serial.print("yo this is actually working bruhhhhhhhhh\n");

//   // for calculating the brightness using BAM
//   // you can think of BAM_Bit as the brightness for BAM: on for 1 tick, off for 2, on for 4, on for 8, etc....
//   // no idea why the numbers are scuffed
//   if(BAM_Counter==8)
//   BAM_Bit++;
//   else
//   if(BAM_Counter==24)
//   BAM_Bit++;
//   else
//   if(BAM_Counter==56)
//   BAM_Bit++;
//   if(BAM_Counter==120){
//   BAM_Counter=0;
//   BAM_Bit=0;
//   }

//   BAM_Counter++;//Here is where we increment the BAM counter

// // Here, we have to do some bitmath to send out both anode + cathode information all at once since
// // we need to send out 1 byte for SPI.transfer() to work. Since anode info is a total of 4 bits and cathode is
// // a total of 4 bits as well, we can combine both pieces of information to create a byte.
// // We wouldn't need to do this if we had an 8x8, but we dont so yea
// switch (BAM_Bit){
// case 0:
//  // "level" here refers to the z-axis. Since we dont have one, treat it as 0 when tracing!
//  for(shift_out=level; shift_out<level+4; shift_out++){
//  // ((rows) | (cols))
//  BAMDataToSend = ((anode[anodelevel]) | (led0[shift_out]));
// //  Serial.print("led0 Data: ");
// //  Serial.print((led0[shift_out]), BIN);
// //  Serial.print("\nanodeData: ");
// //  Serial.print((anode[anodelevel]), BIN);
// //  Serial.print("\nSent Data: ");
// //  Serial.print(BAMDataToSend, BIN);
// //  Serial.print("\n\n");
// //  SPI.transfer(BAMDataToSend);
//  }
//  // when adding RGB later, add back a couple of for loops for green and blue
//  break;
// case 1:
//  // "level" here refers to the z-axis. Since we dont have one, treat it as 0!
//  for(shift_out=level; shift_out<level+4; shift_out++){
//  // ((rows) | (cols))
//  BAMDataToSend = ((anode[anodelevel]) | (led1[shift_out]));
//  SPI.transfer(BAMDataToSend);
//  }
//  // when adding RGB later, add back a couple of for loops for green and blue
//  break;
//  case 2:
//  // "level" here refers to the z-axis. Since we dont have one, treat it as 0!
//  for(shift_out=level; shift_out<level+4; shift_out++){
//  // ((rows) | (cols))
//  BAMDataToSend = ((anode[anodelevel]) | (led2[shift_out]));
 
//  SPI.transfer(BAMDataToSend);
 
//  }
//  // when adding RGB later, add back a couple of for loops for green and blue
//  break;
//  case 3:
//  // "level" here refers to the z-axis. Since we dont have one, treat it as 0!
//  for(shift_out=level; shift_out<level+4; shift_out++){
//  // ((rows) | (cols))
//  BAMDataToSend = ((anode[anodelevel]) | (led3[shift_out]));
//  SPI.transfer(BAMDataToSend);
// //  Serial.print("led3 Data: ");
// //  Serial.print((led3[shift_out]), BIN);
// //  Serial.print("\nanodeData: ");
// //  Serial.print((anode[anodelevel]), BIN);
// // Serial.print("\nSent Data: ");
// // Serial.print(BAMDataToSend, BIN);
// // Serial.print("\n\n");
//  }
//  // when adding RGB later, add back a couple of for loops for green and blue
//  break;
// }//switch_case

// // we would send out anode info here if we had an 8x8x8
// // SPI.transfer(anode[anodelevel]);//finally, send out the anode level byte

// // better pray for this one to work properly cuz i have no idea whats happening here
// // PORTD |= 1<<latchPin;//Latch pin HIGH
// // PORTD &= ~(1<<latchPin);//Latch pin LOW
// // well looks like praying didnt work because i actually had to change this
// PORTB |= latchPinBIN;//Latch pin HIGH
// PORTB &= ~(latchPinBIN);//Latch pin LOW

// anodelevel++;//increment the anode level
// if(anodelevel==4)//go back to 0 if max is reached
// anodelevel=0;

// // Serial.print("BAM Info: \nBAM_Counter: ");
// // Serial.print(BAM_Counter);
// // Serial.print("\nBAM_Bit: ");
// // Serial.print(BAM_Bit);
// // Serial.print("\nanodeLevel: ");
// // Serial.print(anodelevel);
// // Serial.print("\nSent Data: ");
// // Serial.print(BAMDataToSend, BIN);
// // Serial.print("\n\n");

// }
bool didOnce = false;

void allOn(){
  if (!didOnce){
    didOnce = true;
    for(int i = 0; i < 4; i ++){
      for (int j = 0; j < 4; j ++){
        LED(i, j, 0b00001111);
      }
    }
  }
}

void testPattern(){

  // test pattern as shown on Jan 13
  if (!once){
    once = true;
    LED(3, 1, 0b00001111);
    LED(3, 3, 0b00001111);
  }
  delay (2000);
  allOn();

  // Row of LED for counting
  // LED(0, 3, 0b00001111);
  // LED(1, 3, 0b00001111);
  // LED(2, 3, 0b00001111);
  // LED(3, 3, 0b00001111);

  // Strategic distribution of resources - works amazing!
  // LED(0, 0, 0b00000101);
  // LED(0, 1, 0b00000101);
  // LED(0, 2, 0b00000010);
  // LED(0, 3, 0b00001000);

  // LED(1, 0, 0b00000101);
  // LED(1, 1, 0b00000101);
  // LED(1, 2, 0b00000010);
  // LED(1, 3, 0b00001000);

  // LED(2, 0, 0b00000101);
  // LED(2, 2, 0b00000101);
  // LED(2, 1, 0b00000010);
  // LED(2, 3, 0b00001000);

  // LED(3, 0, 0b00000101);
  // LED(3, 2, 0b00000101);
  // LED(3, 1, 0b00000010);
  // LED(3, 3, 0b00001000);


  // Lowest brightness - all loads on 1 bit - doesn't work
  // LED(3, 0, 0b00001111);
  // LED(3, 1, 0b00001111);
  // LED(3, 2, 0b00001111);
  // LED(3, 3, 0b00001111);
}

int phase = 0;
// switches 1 LED between brightness to test that it works
void brightnessSwitcherAnimation(){

  // constant LED
  LED(3, 3, 0b00000001);

  // variable LED
  if (phase == 0){
    LED(2, 3, 0b00001111);
  }
  else{
    LED(2, 3, 0b00000001);
  }

  // reset phase
  phase ++;
  if (phase == 2){
    phase = 0;
  }

  delay(2000);
}


int stage = 0;
// night rider, but using BAM
void nightBAMmer(){

  // when u r too lazy to write a for loop .................
  // omg who wrote this piece of spaghetti
  switch(stage){
    case 0:
      LED(0, 3, 0b00000001);
      LED(1, 3, 0b00000011);
      LED(2, 3, 0b00000111);
      LED(3, 3, 0b00001111);

      LED(3, 2, 0b00000001);

      delay(delayTime);
      stage ++;
    case 1:
      LED(0, 3, 0b00000000);
      LED(1, 3, 0b00000001);
      LED(2, 3, 0b00001111);
      LED(3, 3, 0b00000111);

      LED(3, 2, 0b00000000);

      delay(delayTime);
      stage ++;
    case 2:
      LED(0, 3, 0b00000000);
      LED(1, 3, 0b00001111);
      LED(2, 3, 0b00000111);
      LED(3, 3, 0b00000011);
      delay(delayTime);
      stage ++;
    case 3:
      LED(0, 3, 0b00001111);
      LED(1, 3, 0b00000111);
      LED(2, 3, 0b00000011);
      LED(3, 3, 0b00000001);

      LED(0, 2, 0b00000001);

      delay(delayTime);
      stage ++;
    case 4:
      LED(0, 3, 0b00000111);
      LED(1, 3, 0b00001111);
      LED(2, 3, 0b00000001);
      LED(3, 3, 0b00000000);

      LED(0, 2, 0b00000000);

      delay(delayTime);
      stage ++;
    case 5:
      LED(0, 3, 0b00000011);
      LED(1, 3, 0b00000111);
      LED(2, 3, 0b00001111);
      LED(3, 3, 0b00000000);
      delay(delayTime);
      stage ++;
    case 6: 
      LED(0, 3, 0b00000001);
      LED(1, 3, 0b00000011);
      LED(2, 3, 0b00000111);
      LED(3, 3, 0b00001111);

      LED(3, 2, 0b00000001);

      stage = 0;
  }

}