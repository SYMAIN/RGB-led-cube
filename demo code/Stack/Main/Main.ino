// This code was made for the Arduino Mega2560.
// Anything related to output registers (PORTxx) and SPI pins will need to be changed if using a different Arduino

unsigned char defeatTheCrumbyPreprocessor;
// https://forum.arduino.cc/t/tccr1a-was-not-declared-in-this-scope/177826/4

// The Carry
// https://www.kevindarrah.com/download/8x8x8/RGB_CubeV12_BitwiseFix.ino
// https://www.youtube.com/watch?v=xmScytz9y0M

/*Anoop M M anoopmmkt@gmail.com(Electronics Demon) */

#include <SPI.h> // SPI Library used to clock data out to the shift registers

//pin connections- the #define tag will replace all instances of "latchPin" in your code with A1 (and so on)
#define latch_pin 11 // aka PB5 / PORTB5. This was found on the schematics of the Arduino Mega2560. 
#define latchPinBIN 0b00100000 // this is the internal representation of turning PORTB5 on, aka the latch pin.
#define blankPin 8 // aka PH5 / PORTH5
#define blankPinBIN 0b00100000 // this is the internal representation of turning PH5 on

#define clock_pin 52 // The SPI-MOSI pinout on the Arduino Mega. Varies between Arduinos.
#define data_pin 51 // The SPI-SCK pinout on the Arduino Mega. Varies between Arduinos.

// BLANK: Blue, pin 8
// LATCH: Yellow, pin 11
// DATA: Orange, pin 51
// CLOCK: green, pin 52

int shift_out;
byte anode[4];

//This is how the brightness for every LED is stored,

byte red0[8], red1[8], red2[8], red3[8];
byte blue0[8], blue1[8], blue2[8], blue3[8];
byte green0[8], green1[8], green2[8], green3[8];

int level = 0;                //keeps track of which level we are shifting data to
int anodelevel = 0;           //this increments through the anode levels
int BAM_Bit, BAM_Counter = 0; // Bit Angle Modulation variables to keep track of things

bool stopAnimation = false; // for stopping animation early

unsigned long start; //for a millis timer to cycle through the animations

void setup()
{

  Serial.begin(9600);

  SPI.setBitOrder(MSBFIRST);           //Most Significant Bit First
  SPI.setDataMode(SPI_MODE0);          // Mode 0 Rising edge of data, keep clock low
  SPI.setClockDivider(SPI_CLOCK_DIV2); //Run the data in at 16MHz/2 - 8MHz

  noInterrupts(); // kill interrupts until everybody is set up

  //Timer 1 to refresh the cube
  TCCR1A = B00000000;
  TCCR1B = B00001011;

  TIMSK1 = B00000010;
  OCR1A = 30;

  anode[0]=0b00000001;
  anode[1]=0b00000010;
  anode[2]=0b00000100;
  anode[3]=0b00001000;
  
  pinMode(latch_pin, OUTPUT); //Latch
  pinMode(data_pin, OUTPUT);  //MOSI DATA
  pinMode(clock_pin, OUTPUT); //SPI Clock
  pinMode(blankPin, OUTPUT); //Output Enable  important to do this last, so LEDs do not flash on boot up
  //digitalWrite(blankPin, HIGH);
  SPI.begin();                //start up the SPI library
  interrupts();               //let the show begin, this lets the multiplexing start
}

void loop()
{ 
  //spiral();
  //sparkles();
  fireFireworks();
  //tracer();
  // LED(0, 0, 0, 0, 15, 0);
}

void tracer(){
  for (int j = 0; j < 4; j ++){
    for (int i = 0; i < 4; i ++){
      LED(i, 0, j, 15, 0, 0);
      if (stopAnimation){
        clean();
        return;
      }
      delay(100);
      clean();
    }
    for (int i = 0; i < 4; i ++){
      LED(3, i, j, 0, 15, 0);
      if (stopAnimation){
        clean();
        return;
      }
      delay(100);
      clean();
    }
    for (int i = 3; i != -1; i ++){
      LED(i, 3, j, 0, 0, 15);
      if (stopAnimation){
        clean();
        return;
      }
      delay(100);
      clean();
    }
    for (int i = 3; i != -1; i ++){
      LED(0, i, j, 5, 5, 5);
      if (stopAnimation){
        clean();
        return;
      }
      delay(100);
      clean();
    }
  }
}

void fireFireworks(){
  // start pos
  int startX = random(4);
  int startY = random(4);

  // line
  for (int i = 0; i < 4; i ++){
    // trail
    LED(startX, startY, i - 1, 5, 0, 0);
    LED(startX, startY, i - 2, 1, 0, 0);

    // missle
    LED(startX, startY, i, 0, 0, 10);

    if (stopAnimation){
      clean();
      return;
    }

    delay(300);
    clean();
  }

  // explosion phase1
  LED(startX, startY, 3, 15, 7, 0);
  LED(startX + 1, startY, 3, 15, 7, 0);
  LED(startX - 1, startY, 1, 15, 7, 0);
  LED(startX, startY + 1, 3, 15, 7, 0);
  LED(startX, startY - 1, 2, 15, 7, 0);
  if (stopAnimation){
    clean();
    return;
  }
  delay(200);
  clean();

  // explosion phase2
  LED(startX, startY, 2, 15, 15, 0);
  LED(startX + 1, startY, 2, 15, 15, 0);
  LED(startX - 1, startY, 0, 15, 15, 0);
  LED(startX, startY + 1, 2, 15, 15, 0);
  LED(startX, startY - 1, 1, 15, 15, 0);
  if (stopAnimation){
    clean();
    return;
  }
  delay(200);
  clean();
  
  // wait 2s between fireworks
  if (stopAnimation){
    clean();
    return;
  }
  delay(1000);

  if (stopAnimation){
    clean();
    return;
  }
  delay(1000);
}

void sparkles(){
  LED(random(4), random(4), random(4), random(8) + 8, random(8), random(8));
  LED(random(4), random(4), random(4), random(8), random(8) + 8, random(8));
  LED(random(4), random(4), random(4), random(8), random(8), random(8) + 8);
  if (stopAnimation){
    clean();
    return;
  }
  delay(500);
  clean();
}

void launchMissles(){
  // start pos
  int startX = 1 + random(2);
  int startZ = 1 + random(2);
  // line
  for (int i = 0; i < 4; i ++){
    // trail
    if (i > 0){
      LED(startX, i - 1, startZ, 7, 0, 0);
    }
    // missle
    LED(startX, i, startZ, 0, 0, 15);

    if (stopAnimation){
      clean();
      return;
    }

    delay(200);
    clean();
  }
  // explosion
  LED(startX, 2, startZ, random(15), random(15), random(15));
  LED(startX, 3, startZ, random(15), random(15), random(15));
  LED(startX, 3, startZ + 1, random(15), random(15), random(15));
  LED(startX, 3, startZ - 1, random(15), random(15), random(15));
  LED(startX + 1, 3, startZ, random(15), random(15), random(15));
  LED(startX - 1, 3, startZ, random(15), random(15), random(15));

  if (stopAnimation){
    clean();
    return;
  }

  delay(200);
  clean();

  // falling
  LED(startX, 2, startZ - 1, random(7), random(7), random(7));
  LED(startX, 3, startZ - 1, random(7), random(7), random(7));
  LED(startX, 3, startZ, random(7), random(7), random(7));
  LED(startX + 1, 3, startZ - 1, random(7), random(7), random(7));
  LED(startX - 1, 3, startZ - 1, random(7), random(7), random(7));

  if (stopAnimation){
    clean();
    return;
  }
  
  delay(200);
  clean();
}

void allBlue(){
  // add
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      for (int k = 0; k < 4; k++)
        LED(i, j, k, 0, 0, 15);
}

void allGreen(){
  // blue
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      for (int k = 0; k < 4; k++)
        LED(i, j, k, 0, 15, 0);
}

void allRed(){
  // blue
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      for (int k = 0; k < 4; k++)
        LED(i, j, k, 15, 0, 0);
}

void LED(int row, int column, int level, byte red, byte green, byte blue)
{ //****LED Routine****LED Routine****LED Routine****LED Routine

  // error handling
  if ((level > 3) || (level < 0) || (row > 3) || (row < 0) || (column > 3) || (column < 0)){
    return;
  }

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

bool bruteForce = false;
ISR(TIMER1_COMPA_vect)
{
  // Blank everything while we change some stuff
  //PORTD |= blankPinBIN;//Blank pin HIGH (aka disable outputs)

  if (BAM_Counter == 8)
    BAM_Bit++;
  else if (BAM_Counter == 24)
    BAM_Bit++;
  else if (BAM_Counter == 56)
    BAM_Bit++;

  BAM_Counter++;

  switch (BAM_Bit)
  {
  case 0:
    if (!bruteForce){
      for (shift_out = level; shift_out < level + 2; shift_out++)
        SPI.transfer(blue0[shift_out]);
      for (shift_out = level; shift_out < level + 2; shift_out++)
        SPI.transfer(green0[shift_out]);
      for (shift_out = level; shift_out < level + 2; shift_out++)
        SPI.transfer(red0[shift_out]);
    }

    else{
      SPI.transfer(0b00000001);
      SPI.transfer(0b00000000);
      SPI.transfer(0b00000000);
      SPI.transfer(0b00000000);
      SPI.transfer(0b00000000);
      SPI.transfer(0b00000000);
    }

    break;
  case 1:
    if (!bruteForce){
      for (shift_out = level; shift_out < level + 2; shift_out++)
        SPI.transfer(blue1[shift_out]);
      for (shift_out = level; shift_out < level + 2; shift_out++)
        SPI.transfer(green1[shift_out]);
      for (shift_out = level; shift_out < level + 2; shift_out++)
        SPI.transfer(red1[shift_out]);
    }

    else{
      SPI.transfer(0b00000001);
      SPI.transfer(0b00000000);
      SPI.transfer(0b00000000);
      SPI.transfer(0b00000000);
      SPI.transfer(0b00000000);
      SPI.transfer(0b00000000);
    }
    break;
  case 2:
    if (!bruteForce){
      for (shift_out = level; shift_out < level + 2; shift_out++)
        SPI.transfer(blue2[shift_out]);
      for (shift_out = level; shift_out < level + 2; shift_out++)
        SPI.transfer(green2[shift_out]);
      for (shift_out = level; shift_out < level + 2; shift_out++)
        SPI.transfer(red2[shift_out]);
    }

    else{
      SPI.transfer(0b00000001);
      SPI.transfer(0b00000000);
      SPI.transfer(0b00000000);
      SPI.transfer(0b00000000);
      SPI.transfer(0b00000000);
      SPI.transfer(0b00000000);
    }
    break;

  case 3:
    if (!bruteForce){
      for (shift_out = level; shift_out < level + 2; shift_out++)
        SPI.transfer(blue3[shift_out]);
      for (shift_out = level; shift_out < level + 2; shift_out++)
        SPI.transfer(green3[shift_out]);
      for (shift_out = level; shift_out < level + 2; shift_out++)
        SPI.transfer(red3[shift_out]);
    }

    else{
      SPI.transfer(0b00000001);
      SPI.transfer(0b00000000);
      SPI.transfer(0b00000000);
      SPI.transfer(0b00000000);
      SPI.transfer(0b00000000);
      SPI.transfer(0b00000000);
    }

    if (BAM_Counter == 120)
    {
      BAM_Counter = 0;
      BAM_Bit = 0;
    }
    break;
  }
  
  //SPI.transfer(0b11111111);
  SPI.transfer(anode[anodelevel]); //finally, send out the anode level byte

  // PORTD |= latchPinBIN;//Latch pin HIGH
  // PORTD &= ~(latchPinBIN);//Latch pin LOW
  digitalWrite(latch_pin, HIGH);
  digitalWrite(latch_pin, LOW);
  // turn everything back on by disabling blanks
  //PORTD &= ~(blankPinBIN);//Latch pin LOW

  anodelevel++;
  level = level + 2;

  if (anodelevel == 4)
    anodelevel = 0;
  if (level == 8)
    level = 0;
  //pinMode(blankPin, OUTPUT);
}

void clean()
{
  int ii, jj, kk;
  for (ii = 0; ii < 4; ii++)
    for (jj = 0; jj < 4; jj++)
      for (kk = 0; kk < 4; kk++)
        LED(ii, jj, kk, 0, 0, 0);
}