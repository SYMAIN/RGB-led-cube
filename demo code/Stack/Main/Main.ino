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

  anode[0]=0b11101110;
  anode[1]=0b11011101;
  anode[2]=0b10111011;
  anode[3]=0b01110111;
  
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
  all();
  // movePlane();
  // moveSingle();
  // moveSqure();
  // moveOnePixel();
  // allLeds();
  // planeSwipe();
  // randomLeds();
}
// void randomLeds()
// {
//   int x, y, z, red, green, blue;
//   start = millis();

//   while (millis() - start < 5000)
//   {
//     x = random(4);
//     y = random(4);
//     z = random(4);
//     red = random(16);
//     green = random(16);
//     blue = random(16);
//     LED(x, y, z, red, green, blue);

//     delay(20);
//   }
//   clean();
// }
// void planeSwipe()
// {
//   for (int i = 0; i < 3; i++)
//   {
//     int child = 0;
//     for (int j = 0; j < 4; j++)
//     {
//       if (j >= (3 - i))
//       {
//         for (int k = 0; k < 4; k++)
//           LED(k, j, child, 0, 15, 0);
//         child++;
//       }
//       else
//       {
//         for (int k = 0; k < 4; k++)
//           LED(k, j, child, 0, 15, 0);
//       }
//     }
//     delay(100);
//     clean();
//   }
//   for (int i = 0; i < 4; i++)
//   {
//     int child = 0;
//     for (int j = 0; j < 4; j++)
//     {
//       if (j >= i)
//       {
//         for (int k = 0; k < 4; k++)
//           LED(k, child, j, 0, 15, 0);
//         child++;
//       }
//       else
//       {
//         for (int k = 0; k < 4; k++)
//           LED(k, child, j, 0, 15, 0);
//       }
//     }
//     delay(100);
//     clean();
//   }
//   for (int i = 0; i < 3; i++)
//   {
//     int child = 0;
//     for (int j = 0; j < 4; j++)
//     {
//       if (j >= (3 - i))
//       {
//         for (int k = 0; k < 4; k++)
//           LED(k, child, j, 0, 15, 0);
//         child++;
//       }
//       else
//       {
//         for (int k = 0; k < 4; k++)
//           LED(k, child, j, 0, 15, 0);
//       }
//     }
//     delay(100);
//     clean();
//   }
//   for (int i = 0; i < 4; i++)
//   {
//     int child = 0;
//     for (int j = 0; j < 4; j++)
//     {
//       if (j >= i)
//       {
//         for (int k = 0; k < 4; k++)
//           LED(k, j, child, 0, 15, 0);
//         child++;
//       }
//       else
//       {
//         for (int k = 0; k < 4; k++)
//           LED(k, j, child, 0, 15, 0);
//       }
//     }
//     delay(100);
//     clean();
//   }
//   ///////////////
//   for (int i = 0; i < 3; i++)
//   {
//     int child = 0;
//     for (int j = 0; j < 4; j++)
//     {
//       if (j >= (3 - i))
//       {
//         for (int k = 0; k < 4; k++)
//           LED(j, child, k, 0, 0, 15);
//         child++;
//       }
//       else
//       {
//         for (int k = 0; k < 4; k++)
//           LED(j, child, k, 0, 0, 15);
//       }
//     }
//     delay(100);
//     clean();
//   }
//   for (int i = 0; i < 4; i++)
//   {
//     int child = 0;
//     for (int j = 0; j < 4; j++)
//     {
//       if (j >= i)
//       {
//         for (int k = 0; k < 4; k++)
//           LED(child, j, k, 0, 0, 15);
//         child++;
//       }
//       else
//       {
//         for (int k = 0; k < 4; k++)
//           LED(child, j, k, 0, 0, 15);
//       }
//     }
//     delay(100);
//     clean();
//   }
//   for (int i = 0; i < 3; i++)
//   {
//     int child = 0;
//     for (int j = 0; j < 4; j++)
//     {
//       if (j >= (3 - i))
//       {
//         for (int k = 0; k < 4; k++)
//           LED(child, j, k, 0, 0, 15);
//         child++;
//       }
//       else
//       {
//         for (int k = 0; k < 4; k++)
//           LED(child, j, k, 0, 0, 15);
//       }
//     }
//     delay(100);
//     clean();
//   }
//   for (int i = 0; i < 4; i++)
//   {
//     int child = 0;
//     for (int j = 0; j < 4; j++)
//     {
//       if (j >= i)
//       {
//         for (int k = 0; k < 4; k++)
//           LED(j, child, k, 0, 0, 15);
//         child++;
//       }
//       else
//       {
//         for (int k = 0; k < 4; k++)
//           LED(j, child, k, 0, 0, 15);
//       }
//     }
//     delay(100);
//     clean();
//   }
//   //////////////////////
//   for (int i = 0; i < 3; i++)
//   {
//     int child = 0;
//     for (int j = 0; j < 4; j++)
//     {
//       if (j >= (3 - i))
//       {
//         for (int k = 0; k < 4; k++)
//           LED(j, k, child, 15, 0, 0);
//         child++;
//       }
//       else
//       {
//         for (int k = 0; k < 4; k++)
//           LED(j, k, child, 15, 0, 0);
//       }
//     }
//     delay(100);
//     clean();
//   }
//   for (int i = 0; i < 4; i++)
//   {
//     int child = 0;
//     for (int j = 0; j < 4; j++)
//     {
//       if (j >= i)
//       {
//         for (int k = 0; k < 4; k++)
//           LED(child, k, j, 15, 0, 0);
//         child++;
//       }
//       else
//       {
//         for (int k = 0; k < 4; k++)
//           LED(child, k, j, 15, 0, 0);
//       }
//     }
//     delay(100);
//     clean();
//   }
//   for (int i = 0; i < 3; i++)
//   {
//     int child = 0;
//     for (int j = 0; j < 4; j++)
//     {
//       if (j >= (3 - i))
//       {
//         for (int k = 0; k < 4; k++)
//           LED(child, k, j, 15, 0, 0);
//         child++;
//       }
//       else
//       {
//         for (int k = 0; k < 4; k++)
//           LED(child, k, j, 15, 0, 0);
//       }
//     }
//     delay(100);
//     clean();
//   }
//   for (int i = 0; i < 4; i++)
//   {
//     int child = 0;
//     for (int j = 0; j < 4; j++)
//     {
//       if (j >= i)
//       {
//         for (int k = 0; k < 4; k++)
//           LED(j, k, child, 15, 0, 0);
//         child++;
//       }
//       else
//       {
//         for (int k = 0; k < 4; k++)
//           LED(j, k, child, 15, 0, 0);
//       }
//     }
//     delay(100);
//     clean();
//   }
// }
// void movePlane()
// {
//   start = millis();

//   while (millis() - start < 5000)
//   {
//     for (int i = 0; i < 4; i++)
//     {
//       for (int j = 0; j < 4; j++)
//         for (int k = 0; k < 4; k++)
//         {
//           LED(i, j, k, 15, 0, 0);
//           LED(j, i, k, 0, 10, 0);
//           LED(j, k, i, 0, 0, 10);
//         }

//       delay(100);
//       clean();
//     }
//   }
// }
// void moveSingle()
// {
//   start = millis();

//   while (millis() - start < 5000)
//   {
//     for (int i = 0; i < 4; i++)
//       for (int j = 0; j < 4; j++)
//         for (int k = 0; k < 4; k++)
//         {
//           LED(i, j, k, 15, 0, 15);
//           LED(j, i, k, 15, 0, 15);
//           LED(i, k, j, 0, 15, 15);
//           delay(50);
//           clean();
//         }
//   }
// }
// void moveOnePixel()
// {
//   start = millis();
//   int layer, column, row, red, green, blue;
//   while (millis() - start < 5000)
//   {
//     layer = random(4);
//     column = random(4);
//     row = random(4);
//     red = random(16);
//     green = random(16);
//     blue = random(16);
//     LED(layer, column, row, red, green, blue);
//     LED(column, layer, row, red, green, blue);
//     LED(row, layer, column, red, green, blue);
//     delay(50);
//     clean();
//   }
// }
// void moveSqure()
// {
//   start = millis();

//   while (millis() - start < 5000)
//   {
//     int red = random(15);
//     int green = random(15);
//     int blue = random(15);
//     LED(1, 1, 1, red, green, blue);
//     LED(1, 1, 2, red, green, blue);
//     LED(1, 2, 1, red, green, blue);
//     LED(1, 2, 2, red, green, blue);
//     LED(2, 1, 1, red, green, blue);
//     LED(2, 1, 2, red, green, blue);
//     LED(2, 2, 1, red, green, blue);
//     LED(2, 2, 2, red, green, blue);
//     delay(200);
//     clean();
//     for (int i = 0; i < 4; i++)
//       for (int j = 0; j < 4; j++)
//         LED(i, j, 0, red, green, blue);
//     for (int i = 0; i < 4; i++)
//       for (int j = 0; j < 4; j++)
//         LED(i, 0, j, red, green, blue);
//     for (int i = 0; i < 4; i++)
//       for (int j = 0; j < 4; j++)
//         LED(0, i, j, red, green, blue);
//     for (int i = 0; i < 4; i++)
//       for (int j = 0; j < 4; j++)
//         LED(i, j, 3, red, green, blue);
//     for (int i = 0; i < 4; i++)
//       for (int j = 0; j < 4; j++)
//         LED(i, 3, j, red, green, blue);
//     for (int i = 0; i < 4; i++)
//       for (int j = 0; j < 4; j++)
//         LED(3, i, j, red, green, blue);

//     delay(200);
//     clean();
//   }
// }

void all(){
  // add
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
        for (int k = 0; k < 4; k++)
          LED(i, j, k, 15, 0, 0);
}

void allLeds()
{
  // // red
  // for (int i = 0; i < 4; i++)
  //   for (int j = 0; j < 4; j++)
  //       for (int k = 0; k < 4; k++)
  //         LED(i, j, k, 15, 0, 0);
  // delay(1000);
  // clean();

  // // green
  // for (int i = 0; i < 4; i++)
  //   for (int j = 0; j < 4; j++)
  //       for (int k = 0; k < 4; k++)
  //         LED(i, j, k, 0, 15, 0);
  // delay(1000);
  // clean();

  // // blue
  // for (int i = 0; i < 4; i++)
  //   for (int j = 0; j < 4; j++)
  //       for (int k = 0; k < 4; k++)
  //         LED(i, j, k, 0, 0, 15);
  // delay(1000);
  // clean();

  // if (done && !once0){
  //   once0 = true;
  //   Serial.println("Blue Data Outside Timer");
  //   for (int i = 0; i < 8; i ++){
  //     Serial.println(blue0[i], BIN);
  //   }
  // }
  // delay(1000);
  // clean();

  // for (int brightness = 0; brightness < 16; brightness++)
  // {
  //   for (int i = 0; i < 4; i++)
  //     for (int j = 0; j < 4; j++)
  //       for (int k = 0; k < 4; k++)
  //       {
  //         LED(i, j, k, brightness, 0, 0);
  //       }
  //   delay(30);
  // }
  // delay(300);
  // for (int brightness = 15; brightness >= 0; brightness--)
  // {
  //   for (int i = 0; i < 4; i++)
  //     for (int j = 0; j < 4; j++)
  //       for (int k = 0; k < 4; k++)
  //       {
  //         LED(i, j, k, brightness, 0, 0);
  //       }
  //   delay(30);
  // }
  // for (int brightness = 0; brightness < 16; brightness++)
  // {
  //   for (int i = 0; i < 4; i++)
  //     for (int j = 0; j < 4; j++)
  //       for (int k = 0; k < 4; k++)
  //       {
  //         LED(i, j, k, 0, brightness, 0);
  //       }
  //   delay(30);
  // }
  // delay(300);
  // for (int brightness = 15; brightness >= 0; brightness--)
  // {
  //   for (int i = 0; i < 4; i++)
  //     for (int j = 0; j < 4; j++)
  //       for (int k = 0; k < 4; k++)
  //       {
  //         LED(i, j, k, 0, brightness, 0);
  //       }
  //   delay(30);
  // }
  // for (int brightness = 0; brightness < 16; brightness++)
  // {
  //   for (int i = 0; i < 4; i++)
  //     for (int j = 0; j < 4; j++)
  //       for (int k = 0; k < 4; k++)
  //       {
  //         LED(i, j, k, 0, 0, brightness);
  //       }
  //   delay(30);
  // }
  // delay(300);
  // for (int brightness = 15; brightness >= 0; brightness--)
  // {
  //   for (int i = 0; i < 4; i++)
  //     for (int j = 0; j < 4; j++)
  //       for (int k = 0; k < 4; k++)
  //       {
  //         LED(i, j, k, 0, 0, brightness);
  //       }
  //   delay(30);
  // }
  // Serial.println("blue0");
  // for (int i = 0; i < 8; i++)
  //   Serial.println(blue0[i]);
  // Serial.println("blue1");
  // for (int i = 0; i < 8; i++)
  //   Serial.println(blue1[i]);
  // Serial.println("blue2");
  // for (int i = 0; i < 8; i++)
  //   Serial.println(blue2[i]);
  // Serial.println("blue3");
  // for (int i = 0; i < 8; i++)
  //   Serial.println(blue3[i]);
  // Serial.println("\n\n\n");
}
void LED(int level, int row, int column, byte red, byte green, byte blue)
{ //****LED Routine****LED Routine****LED Routine****LED Routine

  // if (red != 0){
  //   Serial.println("RED! Call:");
  //   Serial.println(level);
  //   Serial.println(row);
  //   Serial.println(column);
  //   Serial.println(red, BIN);
  //   Serial.println(green, BIN);
  //   Serial.println(blue, BIN);
  //   Serial.println("\n");
  // }

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

  // Serial.print("LED: Writing ");
  // Serial.print(blue, BIN);
  // Serial.print(" to ");
  // Serial.print(whichbyte);
  // Serial.print(" ");
  // Serial.println(wholebyte - (8 * whichbyte));
}

ISR(TIMER1_COMPA_vect)
{
  //if (done){
    // if (done && !once1){
    //   once1 = true;
    //   Serial.println("Blue Data Inside Timer");
    //   for (int i = 0; i < 8; i ++){
    //     Serial.println(blue0[i], BIN);
    //   }
    // }

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
      for (shift_out = level; shift_out < level + 2; shift_out++)
        SPI.transfer(blue0[shift_out]);
      for (shift_out = level; shift_out < level + 2; shift_out++)
        SPI.transfer(green0[shift_out]);
      for (shift_out = level; shift_out < level + 2; shift_out++)
        SPI.transfer(red0[shift_out]);
      break;
    case 1:
      for (shift_out = 0; shift_out < 0 + 2; shift_out++)
        SPI.transfer(blue1[shift_out]);
      for (shift_out = 0; shift_out < 0 + 2; shift_out++)
        SPI.transfer(green1[shift_out]);
      for (shift_out = 0; shift_out < 0 + 2; shift_out++)
        SPI.transfer(red1[shift_out]);
      break;
    case 2:
      for (shift_out = 0; shift_out < 0 + 2; shift_out++)
        SPI.transfer(blue2[shift_out]);
      for (shift_out = 0; shift_out < 0 + 2; shift_out++)
        SPI.transfer(green2[shift_out]);
      for (shift_out = 0; shift_out < 0 + 2; shift_out++)
        SPI.transfer(red2[shift_out]);
      break;

    case 3:
      for (shift_out = 0; shift_out < 0 + 2; shift_out++)
        SPI.transfer(blue3[shift_out]);
      for (shift_out = 0; shift_out < 0 + 2; shift_out++)
        SPI.transfer(green3[shift_out]);
      for (shift_out = 0; shift_out < 0 + 2; shift_out++)
        SPI.transfer(red3[shift_out]);

      if (BAM_Counter == 120)
      {
        BAM_Counter = 0;
        BAM_Bit = 0;
      }
      break;
    }

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
  //}
}

void clean()
{
  int ii, jj, kk;
  for (ii = 0; ii < 4; ii++)
    for (jj = 0; jj < 4; jj++)
      for (kk = 0; kk < 4; kk++)
        LED(ii, jj, kk, 0, 0, 0);
}

//void stack()
//{
//  int level = 2;
//
//  // starting block (2 by 2)
//  int red = random(15);
//  int green = random(15);
//  int blue = random(15);
//  while (true)
//  {
//    LED(level, 1, 0, red, green, blue);
//    LED(level, 1, 1, red, green, blue);
//    LED(level, 2, 0, red, green, blue);
//    LED(level, 2, 1, red, green, blue);
//
//    LED(level, 1, 1, red, green, blue);
//    LED(level, 1, 2, red, green, blue);
//    LED(level, 2, 1, red, green, blue);
//    LED(level, 2, 2, red, green, blue);
//
//    LED(level, 1, 2, red, green, blue);
//    LED(level, 1, 3, red, green, blue);
//    LED(level, 2, 2, red, green, blue);
//    LED(level, 2, 3, red, green, blue);
//
//    // go backward
//    LED(level, 1, 1, red, green, blue);
//    LED(level, 1, 2, red, green, blue);
//    LED(level, 2, 1, red, green, blue);
//    LED(level, 2, 2, red, green, blue);
//
//    if (digitalRead(button_pin) == HIGH)
//    {
//
//    }
//  }
//}
