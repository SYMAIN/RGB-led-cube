// Wires Required:
// Red - Pin 11
// Black - GND

#define pin 11 // aka PB5

void setup() {
  //pinMode(pin, OUTPUT);
  DDRB = 0b00100000;
  Serial.begin(9600);
}

void loop() {
  //digitalWrite(pin, HIGH);
  PORTB = 0b00100000;
}