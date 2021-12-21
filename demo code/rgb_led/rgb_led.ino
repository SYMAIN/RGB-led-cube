// C++ code
//
int redPin = 6;
int greenPin = 3;
int bluePin = 5;  
void setup()
{
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
}

void loop(){
  RGB_color(255,0,0); // red
  delay(1000);
  
  RGB_color(5,215,255); // orange
  delay(1000);
  
  RGB_color(255,255,0); // yellow
  delay(1000);
  
  RGB_color(0,255,0); // green
  delay(1000);
  
  RGB_color(0,0,255); // blue
  delay(1000);
  
  RGB_color(160,32,240); // purple
  delay(1000);
}

void RGB_color(int red_light_value, int green_light_value, int blue_light_value)
 {
  analogWrite(redPin, red_light_value);
  analogWrite(greenPin, green_light_value);
  analogWrite(bluePin, blue_light_value);
}
