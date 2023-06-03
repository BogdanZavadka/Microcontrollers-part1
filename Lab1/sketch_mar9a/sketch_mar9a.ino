const int buttonPin = 43;


void setup() {
  DDRA = 0xFF;
  PORTA = 0;
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
  if(digitalRead(buttonPin) == LOW){
    PORTA = 128;
    bool secondTurn = false;
    while(PORTA){
      if(secondTurn){
        delay(650);
        PORTA = 64;
        secondTurn = false;
      }
      delay(650);
      PORTA = PORTA >> 2;
      if(PORTA == 2){
        secondTurn = true;
      }
    }
  }
}
