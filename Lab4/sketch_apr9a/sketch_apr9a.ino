const int buzzerPin = 28;
const int buttonPin1 = 23, buttonPin2 = 24, buttonPin3 = 25, buttonPin4 = 26;

bool isTimerTicking = true;
int tick_counter = 0;
int lastSavedTimeIndex = 0;
int viewingIndex = 0;

unsigned char number[10] = { 0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90 };
unsigned char timesList[6] = { number[0], number[0], number[0], number[0], number[0], number[0] };

struct Time {
  unsigned char second, minute, hour;
};
Time T2 = { 0, 0, 0 };

struct Time savedTime[10];
unsigned char key_cursor = 1;
unsigned char fig_number = 0;

void encodeTime(unsigned char number, unsigned char position);

ISR(TIMER2_COMPA_vect) {
  if (isTimerTicking) {
    if (tick_counter == 240) {
      if (++T2.second == 60) {
        T2.second = 0;
        if (++T2.minute == 60) {
          T2.minute = 0;
          if (++T2.hour == 24)
            T2.hour = 0;
          encodeTime(T2.hour, 0);
        }
        encodeTime(T2.minute, 2);
      }
      encodeTime(T2.second, 4);
      tick_counter = 0;
    } else {
      tick_counter++;
    }
  }
}

ISR(TIMER0_COMPA_vect) {
  key_cursor <<= 1;
  key_cursor &= 0b00111111;
  if (key_cursor == 0) key_cursor = 1;

  fig_number++;
  if (fig_number == 6) fig_number = 0;

  if (fig_number == 1 || fig_number == 3) {
    timesList[fig_number] ^= 0b10000000;
  }

  PORTD = key_cursor;
  PORTB = timesList[fig_number];
}

void setup() {
  noInterrupts();

  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);
  pinMode(buttonPin3, INPUT_PULLUP);
  pinMode(buttonPin4, INPUT_PULLUP);

  DDRD = 0xFF;
  PORTD = key_cursor;

  DDRB = 0xFF;
  PORTB = 0xFF;

  ASSR |= (1 << EXCLK);
  ASSR |= (1 << AS2);
  OCR2A = 0x1F;
  TCCR2A = 0x00 | (1 << WGM21);
  TCCR2B = (1 << CS22) | (1 << CS20);
  TIMSK2 = (1 << OCIE2A);

  OCR0A = 3;
  TCCR0A = (1 << WGM01);
  TCCR0B = (1 << CS02) | (1 << CS00);
  TIMSK0 |= (1 << OCIE0A);

  interrupts();
}

void loop() {

  if (isTimerTicking && timesList[4] == 0xc0 && timesList[5] == 0xc0) {
    digitalWrite(buzzerPin, HIGH);
    delay(200);
    digitalWrite(buzzerPin, LOW);
  }

  if (digitalRead(buttonPin1) == LOW) {
    viewingIndex = 0;
    if (isTimerTicking) {
      isTimerTicking = false;
    } else {
      T2 = { 0, 0, 0 };
      isTimerTicking = true;
    }
    digitalWrite(buzzerPin, HIGH);
    delay(25000);
    digitalWrite(buzzerPin, LOW);
  }

  if (digitalRead(buttonPin2) == LOW) {
    if (lastSavedTimeIndex < 10) {
      savedTime[lastSavedTimeIndex].hour = T2.hour;
      savedTime[lastSavedTimeIndex].minute = T2.minute;
      savedTime[lastSavedTimeIndex].second = T2.second;
      lastSavedTimeIndex += 1;
      digitalWrite(buzzerPin, HIGH);
      delay(25000);
      digitalWrite(buzzerPin, LOW);
    }
  }

  if (digitalRead(buttonPin3) == LOW) {
    lastSavedTimeIndex = 0;
    delay(25000);
  }

  if (digitalRead(buttonPin4) == LOW && !(isTimerTicking) && lastSavedTimeIndex > 0) {
    if (viewingIndex == lastSavedTimeIndex) viewingIndex = 0;
    encodeTime(savedTime[viewingIndex].hour, 0);
    encodeTime(savedTime[viewingIndex].minute, 2);
    encodeTime(savedTime[viewingIndex].second, 4);
    viewingIndex += 1;
    digitalWrite(buzzerPin, HIGH);
    delay(25000);
    digitalWrite(buzzerPin, LOW);
  }
}

void encodeTime(unsigned char number_in, unsigned char position) {
  unsigned char digitL = number_in;
  unsigned char digitH = 0;
  while (digitL >= 10) {
    digitL -= 10;
    digitH++;
  }
  timesList[position] = number[digitH];
  timesList[position + 1] = number[digitL];
}