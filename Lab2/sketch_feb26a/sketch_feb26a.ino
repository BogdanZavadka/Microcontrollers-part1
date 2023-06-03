const int buttonPin1 =  45;
const int buttonPin2 =  43;
int inByte;

void setup() {
  DDRA = 0xFF;
  PORTA = 0;
  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  if (Serial.available()) {
    inByte = Serial.read();
    if (inByte == 0xA1) {
      //Algorithm 6
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
    } else if (inByte == 0xA2) {
      //Algorithm 8
      PORTA = B00000001;
      delay(650);
      PORTA = B10000000;
      delay(650);
      PORTA = B00000010;
      delay(650);
      PORTA = B01000000;
      delay(650);
      PORTA = B00000100;
      delay(650);
      PORTA = B00100000;
      delay(650);
      PORTA = B00001000;
      delay(650);
      PORTA = B00010000;
      delay(650);
      PORTA = 0;
    }
  }

  if (digitalRead(buttonPin1) == LOW) {
    Serial.write(0xB1);
    delay(650);
  }
  if (digitalRead(buttonPin2) == LOW) {
    Serial.write(0xB2);
    delay(650);
  }
}
