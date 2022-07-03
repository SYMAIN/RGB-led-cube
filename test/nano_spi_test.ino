// need to search up SPI tutorial first









// This code was made for the Arduino Nano.
// Source (pick the Type-C version): https://www.aliexpress.com/item/32241679858.html?spm=a2g0o.productlist.0.0.2d4f5756Ad4QhK&algo_pvid=70d50651-fddb-4330-ad9b-2a7773e2541b&algo_exp_id=70d50651-fddb-4330-ad9b-2a7773e2541b-3&pdp_ext_f=%7B%22sku_id%22%3A%2265479400362%22%7D&pdp_npi=2%40dis%21CAD%21%215.96%21%21%211.81%21%21%402101e9d516567311247102397e0337%2165479400362%21sea

unsigned char defeatTheCrumbyPreprocessor;
// https://forum.arduino.cc/t/tccr1a-was-not-declared-in-this-scope/177826/4

// The Carry
// https://www.kevindarrah.com/download/8x8x8/RGB_CubeV12_BitwiseFix.ino
// https://www.youtube.com/watch?v=xmScytz9y0M

#include <SPI.h> // SPI Library used to clock data out to the shift registers

//pin connections
#define latch_pin 12 // aka D12

#define clock_pin 11 // D11. The SPI-MOSI pinout on the Arduino Nano. Varies between Arduinos.
#define data_pin 13 // D13. The SPI-SCK pinout on the Arduino Nano. Varies between Arduinos.

void setup(){

  Serial.begin(9600);

  SPI.setBitOrder(MSBFIRST);           //Most Significant Bit First
  SPI.setDataMode(SPI_MODE0);          // Mode 0 Rising edge of data, keep clock low
  SPI.setClockDivider(SPI_CLOCK_DIV2); //Run the data in at 16MHz/2 - 8MHz

  noInterrupts(); // kill interrupts until everybody is set up

  // // Timer 1 to refresh the cube.
  // TCCR1A = B00000000;
  // TCCR1B = B00001011;

  // TIMSK1 = B00000010;
  // OCR1A = 30;
  
  pinMode(latch_pin, OUTPUT); //Latch
  pinMode(data_pin, OUTPUT);  //MOSI DATA
  pinMode(clock_pin, OUTPUT); //SPI Clock
  
  SPI.begin();                //start up the SPI library
  interrupts();               //let the show begin, this lets the multiplexing start
}

void loop(){
  SPI.transfer(0b10011001);
  digitalWrite(latch_pin, HIGH);
  digitalWrite(latch_pin, LOW);
  delay(100);
} 

// // confirmed working 
// ISR(TIMER1_COMPA_vect){
//   Serial.println("Working!");
// }