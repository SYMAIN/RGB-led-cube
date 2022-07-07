// Works, although there is some weird behaviour between the timer and the loop() function. 
// It looks like the CPU may be bottlenecking operations here. 
// OCR1A timer was slowed down from 30 -> 300, making it 10x slower than in the 4x4x4.
// I believe it is a CPU bottleneck due to the fact that setting the timer to 30 makes the loop() code execute significantly slower. 
// However, setting it to 300 creates identical performance to code without the timer. 

// This code was made for the Arduino Nano.
// Source (pick the Type-C version): https://www.aliexpress.com/item/32241679858.html?spm=a2g0o.productlist.0.0.2d4f5756Ad4QhK&algo_pvid=70d50651-fddb-4330-ad9b-2a7773e2541b&algo_exp_id=70d50651-fddb-4330-ad9b-2a7773e2541b-3&pdp_ext_f=%7B%22sku_id%22%3A%2265479400362%22%7D&pdp_npi=2%40dis%21CAD%21%215.96%21%21%211.81%21%21%402101e9d516567311247102397e0337%2165479400362%21sea

unsigned char defeatTheCrumbyPreprocessor;
// https://forum.arduino.cc/t/tccr1a-was-not-declared-in-this-scope/177826/4

// The Carry
// https://www.kevindarrah.com/download/8x8x8/RGB_CubeV12_BitwiseFix.ino
// https://www.youtube.com/watch?v=xmScytz9y0M

#include <SPI.h> // SPI Library used to clock data out to the shift registers

//pin connections
#define latch_pin 9 // aka D9. Any pin should work, though try to avoid D10, as it looks like it is another SPI pin used for incoming communications.
#define data_pin 11 // D11. The SPI-MOSI pinout on the Arduino Nano. Varies between Arduinos.
#define clock_pin 13 // D13. The SPI-SCK pinout on the Arduino Nano. Varies between Arduinos.

/*
The data for grounding the LEDs
Each byte for of data represents the data to send out to make a specific level (z-axis) turn on
i.e. : anode[0] is 0b00000001. This contains data to turn on the power for the leds on the bottom level.
*/
byte anode[8]; 

/*
This is how the brightness for every LED is stored.
Each bit in the arrays represents an led in the 8x8x8 cube.
For example, looking at red0[64], (8 leds * 8 leds * 8 leds) = (64 bytes * 8 bits per byte)

To control the brightness, multiple of these byte arrays are used.
Using red0[], you can specify which LED to turn on for 1 unit of time.
With red1[], you can specify which LED to turn on for 2 units of time.
With red2[], you can specify which LED to turn on for 4 units of time.
With red3[], 8 units of time. 
The total time to complete a loop is 15 units of time.

If you wanted to turn on the reds in an LED for ~50% of the time, you'd need to hit 7 or 8 units of time per loop.
In order to hit 7 units, you can set red0[], red1[], and red2[] to on. (1 unit of time + 2 + 4)
To get 8 units of time, you can set red3[] to on (8 units of time)
*/
byte red0[64], red1[64], red2[64], red3[64];
byte blue0[64], blue1[64], blue2[64], blue3[64];
byte green0[64], green1[64], green2[64], green3[64];

/*
Determines if the data sent should be for red0[], red1[], red2[], or red3[]
(Basically controls the brightness inside of the BAM)
Kevin's equivalent variable is BAM_Bit
Values are [0,3]
*/
int brightnessIndex = 0;

/*
Is a global counter to track how many times the timer has executed.
Is used for tracking time to switch between brightnesses inside the timer/BAM.
Kevin's equivalent variable is BAM_Counter
*/
int timerCounter = 0;

// The level to shift data to during BAM. Values are [0,7]
int level = 0;

// Animation support variable.
bool firstRun = true;

// Startup the SPI library and pins
void setup(){

    Serial.begin(9600);

    // messy, but works
    // Retrieves data to power a certain level of LEDs to power. 
    anode[0] = 0b00000001;
    anode[1] = 0b00000010;
    anode[2] = 0b00000100;
    anode[3] = 0b00001000;
    anode[4] = 0b00010000;
    anode[5] = 0b00100000;
    anode[6] = 0b01000000;
    anode[7] = 0b10000000;

    SPI.setBitOrder(MSBFIRST);           //Most Significant Bit First
    SPI.setDataMode(SPI_MODE0);          // Mode 0 Rising edge of data, keep clock low
    SPI.setClockDivider(SPI_CLOCK_DIV2); //Run the data in at 16MHz/2 - 8MHz

    noInterrupts(); // kill interrupts until everybody is set up

    // Timer 1 to refresh the cube.
    TCCR1A = B00000000;
    TCCR1B = B00001011;

    TIMSK1 = B00000010;
    OCR1A = 300;
    
    pinMode(latch_pin, OUTPUT); //Latch
    pinMode(data_pin, OUTPUT);  //MOSI DATA
    pinMode(clock_pin, OUTPUT); //SPI Clock
    
    SPI.begin();                //start up the SPI library
    interrupts();               //let the show begin, this lets the multiplexing start
}

void loop(){
    // Clear data on first run.
    if (firstRun){
        clear();
    }

    // Animations Here ------------------------------
    allOn();
    // Animations End -------------------------------
    
    // Animation support variable. Turn it off after first run.
    firstRun = false;
} 

// The timer for controlling BAM.
ISR(TIMER1_COMPA_vect){
    // Counter variable to save memory.
    // Declared locally to avoid variable collisions outside this function, important for animations.
    int i = 0;

    // Do updates on data to get what to do next

    // Update the brightnesses after a certain amount of time spent/times executed in a brightness mode is achieved. 
    if (timerCounter == 8){ 
        brightnessIndex = 1;
    } // 8 executions spent in lowest brightness mode (0)
    else if (timerCounter == 16){ 
        brightnessIndex = 2;
    } // 8 executions spent in 2nd lowest brightness mode (1)
    else if (timerCounter == 32){ 
        brightnessIndex = 3;
    } // 16 executions spent in 2nd highest brightness mode (2)
    else if (timerCounter == 64){ // This reset is written slight differently to Kevin's reset .... Can copy if doesnt work
        brightnessIndex = 0;
        timerCounter = 0;
    } // 32 executions spent in highest brightness mode (3)

    // Might need to add a blank pin thingy here to disable outputs...

    // Changes from using the least impact brightness arrays like red0[] to higher ones like red3[]
    // This allows switching between different LEDs in an order to allow our eyes to percieve motion in the cube better. 
    switch(brightnessIndex){
        // Lowest brightness setting. Each LED turned on here gets 1 unit of time.
        case 0:
            // Shift out R/G/B data. 
            // This for loop simply shifts out all the red data given the level of LEDs.
            // Level is multiplied by 8 here, since in order to move up a level in the cube, you need to bypass 8 bytes.
            // This is because in red0[], the first 8 bytes represent the first level. 
            // So in order to access the level above the 0th level, you would need to do (level 1) * (8 bytes) to get the starting index the access the bytes [8-15].
            for (i = (level * 8); i < (level * 8) + 8; i ++){
                SPI.transfer(red0[i]);
            }
           for (i = (level * 8); i < (level * 8) + 8; i ++){
                SPI.transfer(green0[i]);
            }
           for (i = (level * 8); i < (level * 8) + 8; i ++){
                SPI.transfer(blue0[i]);
            }
        // Brightness setting. Each LED turned on here gets 2 units of time.
        case 1:
            // Shift out R/G/B data. 
            // This for loop simply shifts out all the red data given the level of LEDs.
            for (i = (level * 8); i < (level * 8) + 8; i ++){
                SPI.transfer(red1[i]);
            }
           for (i = (level * 8); i < (level * 8) + 8; i ++){
                SPI.transfer(green1[i]);
            }
           for (i = (level * 8); i < (level * 8) + 8; i ++){
                SPI.transfer(blue1[i]);
            }
        // Brightness setting. Each LED turned on here gets 4 units of time.
        case 2: 
            // Shift out R/G/B data. 
            // This for loop simply shifts out all the red data given the level of LEDs.
            for (i = (level * 8); i < (level * 8) + 8; i ++){
                SPI.transfer(red2[i]);
            }
           for (i = (level * 8); i < (level * 8) + 8; i ++){
                SPI.transfer(green2[i]);
            }
           for (i = (level * 8); i < (level * 8) + 8; i ++){
                SPI.transfer(blue2[i]);
            }
        // Highest brightness setting. Each LED turned on here gets 8 units of time.
        case 3:
            // Shift out R/G/B data. 
            // This for loop simply shifts out all the red data given the level of LEDs.
            for (i = (level * 8); i < (level * 8) + 8; i ++){
                SPI.transfer(red3[i]);
            }
           for (i = (level * 8); i < (level * 8) + 8; i ++){
                SPI.transfer(green3[i]);
            }
           for (i = (level * 8); i < (level * 8) + 8; i ++){
                SPI.transfer(blue3[i]);
            }
    }

    // Finally, transfer the data for the anode pins after all the data for the LED pins.
    SPI.transfer(anode[level]);

    // Tracking data updates
    timerCounter ++;
    level ++;

    if (level == 8){
        level = 0;
    }

    // Push new data. Might be a bit too slow here ....
    // Might need to find the direct registers inside the Arduino Nano to manipulate
    digitalWrite(latch_pin, HIGH);
    digitalWrite(latch_pin, LOW);
    // Might need to unblank pins here ...
}

/** void LED(int row, int column, int level, byte red, byte green, byte blue)
  * 
  * A function to set a specific LED to light up with a specific intesity of red/green/blue.
  * 
  * @param level, int. The z-axis or layer the LED is on. Supports [0-7].
  * @param row, int. The index of the LED row. Supports [0-7]. For getting a byte from the byte array. (aka led0, led1, led2, led3)
  * @param column, int. The index of the led in the row to light up. Supports [0-7]. For accessing a bit in a byte.
  * @param red, byte. 
  *         Controls bit angle modulation for the color red. Only the 4 least significant bits are used.
  *         It is recommended to supply an int of [0, 15] to specify the brightness (15 being on all the time) instead of a byte.
  *         Example:
  *         If red was given a value of 0b00001010, red1[] and red3[] would be assigned a value of 0 to update the brightness.
  *         (Remember: 0 means no power. The color pin of an anode array need to have no power in order to turn on.)
  * @param green, byte. 
  *         Controls bit angle modulation for the color green. Only the 4 least significant bits are used.
  *         It is recommended to supply an int of [0, 15] to specify the brightness (15 being on all the time) instead of a byte.
  * @param blue, byte.
  *         Controls bit angle modulation for the color blue. Only the 4 least significant bits are used.
  *         It is recommended to supply an int of [0, 15] to specify the brightness (15 being on all the time) instead of a byte.
  *
  */
void LED(int row, int column, int level, byte red, byte green, byte blue){ 

    // Faster code, but less descriptive
    if ((level > 7) || (level < 0) || (row > 7) || (row < 0) || (column > 7) || (column < 0)){
        Serial.println("ERR: Incorrect dimension(s) specified in LED()");
        return;
    }

    // error handling
    // if ((level > 7) || (level < 0)){
    //     Serial.println("ERR: Incorrect 'level' value specified in LED()");
    //     return;
    // }
    // if ((row > 7) || (row < 0)){
    //     Serial.println("ERR: Incorrect 'row' value specified in LED()");
    //     return;
    // }
    // if ((column > 7) || (column < 0)){
    //     Serial.println("ERR: Incorrect 'column' value specified in LED()");
    //     return;
    // }

    // this shows which byte the LED resides in red0[] (and all other associated arrays) 
    int whichbyte = int(((level * 64) + (row * 8) + column) / 8);
    // the index inside the byte where the led resides in red0[] (and all other associated arrays)
    // this should work.... Kevin has alternate code if this doesnt work.
    int ledindex = (((level * 64) + (row * 8) + column)) % 8;

    // bitWrite(brightnessByteToWriteTo, LED, brightness);
    // Note: brightness is flipped, noted by the !.
    // Example: Brightness 1 specified for red in the red byte. We need to write a 0 to red0[].
    // Brightness 1 is 0b....0001. We need to flip the last 4 bits to get the desired output, 0b....1110. 
    bitWrite(red0[whichbyte], ledindex, !bitRead(red, 0));
    bitWrite(red1[whichbyte], ledindex, !bitRead(red, 1));
    bitWrite(red2[whichbyte], ledindex, !bitRead(red, 2));
    bitWrite(red3[whichbyte], ledindex, !bitRead(red, 3));

    bitWrite(green0[whichbyte], ledindex, !bitRead(green, 0));
    bitWrite(green1[whichbyte], ledindex, !bitRead(green, 1));
    bitWrite(green2[whichbyte], ledindex, !bitRead(green, 2));
    bitWrite(green3[whichbyte], ledindex, !bitRead(green, 3));

    bitWrite(blue0[whichbyte], ledindex, !bitRead(blue, 0));
    bitWrite(blue1[whichbyte], ledindex, !bitRead(blue, 1));
    bitWrite(blue2[whichbyte], ledindex, !bitRead(blue, 2));
    bitWrite(blue3[whichbyte], ledindex, !bitRead(blue, 3));
}

// ---------------------------------------
// Animations
// ---------------------------------------

// Clears data in all LEDs.
void clear(){
    for (int r = 0; r < 8; r ++){
        for (int c = 0; c < 8; c ++){
            for (int h = 0; h < 8; h ++){
                LED(r, c, h, 0, 0, 0);
            }
        }
    }
}

// Turns every led for every color for every brightness on.
void allOn(){
    if (firstRun){
        for (int r = 0; r < 8; r ++){
            for (int c = 0; c < 8; c ++){
                for (int h = 0; h < 8; h ++){
                    LED(r, c, h, 15, 15, 15);
                }
            }
        }
    }
}