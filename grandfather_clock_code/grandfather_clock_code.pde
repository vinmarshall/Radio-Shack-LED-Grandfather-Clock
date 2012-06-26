void setup() {                
  // initialize the digital pin as an output.
  // Pin 13 has an LED connected on most Arduino boards:
  pinMode(9, OUTPUT);     
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
}

  int RED_PIN = 9;
  int GREEN_PIN = 11;
  int BLUE_PIN = 10;
  
  int red = 0;
  int green = 0;
  int blue = 0;
  
  int fadeStep = 5; 

void loop() {
  int i;
  for (i=9; i < 12; i++) {
  
    // reset to dark
    analogWrite(RED_PIN, 0);
    analogWrite(GREEN_PIN, 0);
    analogWrite(BLUE_PIN, 0);
  
    analogWrite(i, 255);
    delay(10000);
  }
  
}
