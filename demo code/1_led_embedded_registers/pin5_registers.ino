//pin connections- the #define tag will replace all instances of "latchPin" in your code with A1 (and so on)
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