// works, mostly
// I had issues with QA, the output pin on the opposite side of all the others.
// I'm pretty sure this is something to do with the circuit/electrical rules itself or the shift register though, so the code and concepts should be fine. 

// This code was made for the Arduino Nano.
// Exact arduino (pick the Type-C version): https://www.aliexpress.com/item/32241679858.html?spm=a2g0o.productlist.0.0.2d4f5756Ad4QhK&algo_pvid=70d50651-fddb-4330-ad9b-2a7773e2541b&algo_exp_id=70d50651-fddb-4330-ad9b-2a7773e2541b-3&pdp_ext_f=%7B%22sku_id%22%3A%2265479400362%22%7D&pdp_npi=2%40dis%21CAD%21%215.96%21%21%211.81%21%21%402101e9d516567311247102397e0337%2165479400362%21sea

unsigned char defeatTheCrumbyPreprocessor;
// https://forum.arduino.cc/t/tccr1a-was-not-declared-in-this-scope/177826/4

// The Carry
// https://www.kevindarrah.com/download/8x8x8/RGB_CubeV12_BitwiseFix.ino
// https://www.youtube.com/watch?v=xmScytz9y0M

//pin connections
#define latch_pin 12 // aka pin D12

#define clock_pin 11 // D11.
#define data_pin 13 // D13.

void setup(){

  Serial.begin(9600);
  
  pinMode(latch_pin, OUTPUT); 
  pinMode(data_pin, OUTPUT);  
  pinMode(clock_pin, OUTPUT); 

}

// writeType
// if true, writes HIGH to data pin. Otherwise, LOW.
void write(bool writeType){
    if (writeType == true){
        digitalWrite(data_pin, HIGH);
    }
    else {
        digitalWrite(data_pin, LOW);
    }

    digitalWrite(clock_pin, HIGH);
    digitalWrite(clock_pin, LOW);
}

// turns on/off the latch pin to update new information into the shift register
void update(){
    digitalWrite(latch_pin, HIGH);
    digitalWrite(latch_pin, LOW);
}

bool toWrite = true;
bool firstTime = true;

void loop(){
    // automatically purge data on restart
    if (firstTime){
        clear();
    }

    // animation functions

    // bruteForceTester();
    // oneLED();
    bouncingDoubleLines();

    // animation support variable
    firstTime = false;
} 

void clear(){
    for (int i = 0; i < 8; i ++){
        write(false);
    }
    update();
}

void bruteForceTester(){
    write(true);
    write(false);
    write(true);
    write(false);
    write(true);
    write(true);
    write(true);
    write(false);
    update();
}

// test animations
void oneLED(){
    // turn first led on for first time
    if (firstTime){
        write(true);
    }
    // shift the led over by one each repeat
    else{
        write(false);
    }

    // update latch pin and delay
    update();
    delay(1000);
}

int point1 = 1;
int point2 = 8;

void bouncingDoubleLines(){

    // start updates after first run
    if (!firstTime){
        point1 ++;
        point2 --;
    }

    if (point1 == 9){
        point1 = 2;
    }
    if (point2 == 0){
        point2 = 7; 
    }

    for (int i = 1; i < 9; i ++){
        if ((i == point1) || (i == point2)){
            write(true);
            Serial.println(point1);
        }
        else{
            write(false);
        }
    }

    update();
    delay(500);
    
}