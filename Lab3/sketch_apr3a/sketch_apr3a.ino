#include <LiquidCrystal.h>
#define DDR_KEYPAD  DDRK
#define PORT_KEYPAD PORTK
#define PIN_KEYPAD  PINK
#define AMOUNT_OF_SAVINGS 50
#include "keypad4x4.h"

bool isTimerTicking = false;
bool isUserViewingTimeList = false;
int nextSavedTimeIndex = 0;
int timeListOutputIndex = 0;
int secondDigit = 1;
int choosedNumber = 0;

const int rs = 54, rw = 55, en = 56, d4 = 57, d5 = 58, d6 = 59, d7 = 60;
LiquidCrystal lcd(rs, rw, en, d4, d5, d6, d7);
const int buzzerPin = 21;

const PROGMEM  char sixty[61][3] = {
  {"00"}, {"01"}, {"02"}, {"03"}, {"04"}, {"05"}, {"06"}, {"07"}, {"08"}, {"09"},
  {"10"}, {"11"}, {"12"}, {"13"}, {"14"}, {"15"}, {"16"}, {"17"}, {"18"}, {"19"},
  {"20"}, {"21"}, {"22"}, {"23"}, {"24"}, {"25"}, {"26"}, {"27"}, {"28"}, {"29"},
  {"30"}, {"31"}, {"32"}, {"33"}, {"34"}, {"35"}, {"36"}, {"37"}, {"38"}, {"39"},
  {"40"}, {"41"}, {"42"}, {"43"}, {"44"}, {"45"}, {"46"}, {"47"}, {"48"}, {"49"},
  {"50"}, {"51"}, {"52"}, {"53"}, {"54"}, {"55"}, {"56"}, {"57"}, {"58"}, {"59"}, {"60"}
};

struct Time
{
  unsigned char second, minute, hour;
};
Time T2 = {0, 0, 0};

struct Time savedTimes[AMOUNT_OF_SAVINGS];

void LCD_WriteStrPROGMEM(char *str, int n)
{
  for (int i = 0; i < n; i++)
    lcd.print( (char)pgm_read_byte( &(str[i]) ) );
}

void LCD_WriteSavedTimePROGMEM(int n, int index){
  lcd.setCursor(1, (int)(index % 2));
  LCD_WriteNumber(index);
  lcd.write("  ");
  LCD_WriteStrPROGMEM(sixty[savedTimes[index].hour], n);
  lcd.write(':');
  LCD_WriteStrPROGMEM(sixty[savedTimes[index].minute], n);
  lcd.write(':');
  LCD_WriteStrPROGMEM(sixty[savedTimes[index].second], n);
}

void LCD_WriteNumber(int number){
  if(number + 1 < 10){
    lcd.print( (char)pgm_read_byte( &(sixty[number + 1][1]) ) );
  } else {
    LCD_WriteStrPROGMEM(sixty[number + 1], 2);
  }
}

ISR(TIMER3_COMPA_vect)
{
  if(isTimerTicking){
    if (++T2.second == 60)
    {
      T2.second = 0;
      if (++T2.minute == 60)
      {      
        T2.minute = 0;
        if (++T2.hour == 24)
          T2.hour = 0;
      }
    }
    lcd.setCursor(3, 0);
    LCD_WriteStrPROGMEM(sixty[T2.hour], 2);
    lcd.write(':');
    LCD_WriteStrPROGMEM(sixty[T2.minute], 2);
    lcd.write(':');
    LCD_WriteStrPROGMEM(sixty[T2.second], 2);
  }
}

void setup() {
  noInterrupts();

  TCCR3A = 0x00;
  TCCR3B = (1 << WGM12) | (1 << CS12) | (1 << CS10); //CTC mode & Prescaler @ 1024
  TIMSK3 = (1 << OCIE3A); // дозвіл на переривання по співпадінню
  OCR3A = 0x3D08;// compare value = 1 sec (16MHz AVR)
  interrupts();
    
  initKeyPad();
  lcd.begin(16, 2);
  lcd.setCursor(3, 0);
  lcd.print("00:00:00");

  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);
}

void loop() {
  if((T2.second + 1) == 60){
    digitalWrite(buzzerPin, HIGH);
    delay(1000);
    digitalWrite(buzzerPin, LOW);
  }
  if ( isButtonPressed() ) {
    char pressedButton = readKeyFromPad4x4();     
    if( pressedButton == 'D' ){
      if(!isUserViewingTimeList){
        isTimerTicking = !isTimerTicking;
      }
    }else if( pressedButton == 'C' ){
      if(!isUserViewingTimeList){
        T2.second = 0;
        T2.minute = 0;
        T2.hour = 0;
        lcd.setCursor(3, 0);
        LCD_WriteStrPROGMEM(sixty[T2.hour], 2);
        lcd.write(':');
        LCD_WriteStrPROGMEM(sixty[T2.minute], 2);
        lcd.write(':');
        LCD_WriteStrPROGMEM(sixty[T2.second], 2);
        isTimerTicking = false;
      }
    }else if ( pressedButton == 'E' ){
      if(isUserViewingTimeList){
        if(choosedNumber > 0 && choosedNumber <= nextSavedTimeIndex){
          lcd.clear();
          T2.second = savedTimes[choosedNumber].second;
          T2.minute = savedTimes[choosedNumber].minute;
          T2.hour = savedTimes[choosedNumber].hour;
          lcd.setCursor(3, 0);
          LCD_WriteStrPROGMEM(sixty[T2.hour], 2);
          lcd.write(':');
          LCD_WriteStrPROGMEM(sixty[T2.minute], 2);
          lcd.write(':');
          LCD_WriteStrPROGMEM(sixty[T2.second], 2);
          digitalWrite(buzzerPin, HIGH);
          delay(200);
          digitalWrite(buzzerPin, LOW);
          delay(500);          
          digitalWrite(buzzerPin, HIGH);
          delay(200);
          digitalWrite(buzzerPin, LOW);
          isUserViewingTimeList = 0;
          secondDigit = 1;
          choosedNumber = 0;
          isUserViewingTimeList = false;
          lcd.setCursor(0, 1);
          if(nextSavedTimeIndex > 0){
            lcd.write('M');
            LCD_WriteNumber(nextSavedTimeIndex - 1);
          }
        } else {
          secondDigit = 1;
          choosedNumber = 0;
          digitalWrite(buzzerPin, HIGH);
          delay(1000);
          digitalWrite(buzzerPin, LOW);          
        }
      }else{
        if(nextSavedTimeIndex < 60){
          savedTimes[nextSavedTimeIndex].hour = T2.hour;
          savedTimes[nextSavedTimeIndex].minute = T2.minute;
          savedTimes[nextSavedTimeIndex].second = T2.second;
          nextSavedTimeIndex += 1;
          lcd.setCursor(0, 1);
          lcd.write('M');
          LCD_WriteNumber(nextSavedTimeIndex - 1);
        }        
      }
    } else if (pressedButton == 'F'){
      nextSavedTimeIndex = 0;
      lcd.setCursor(0, 1);
      lcd.write("   ");
    } else if ( pressedButton == 'A'){
      lcd.clear();
      secondDigit = 1;
      choosedNumber = 0;
      if(isUserViewingTimeList){
        lcd.setCursor(3, 0);
        LCD_WriteStrPROGMEM(sixty[T2.hour], 2);
        lcd.write(':');
        LCD_WriteStrPROGMEM(sixty[T2.minute], 2);
        lcd.write(':');
        LCD_WriteStrPROGMEM(sixty[T2.second], 2);
        timeListOutputIndex = 0;
        lcd.setCursor(0, 1);
        if(nextSavedTimeIndex > 0){
          lcd.write('M');
          LCD_WriteNumber(nextSavedTimeIndex - 1);
        }
      }
      else{
        for(int i = 0; i < 2; i++){
          if(timeListOutputIndex < nextSavedTimeIndex){
            LCD_WriteSavedTimePROGMEM(2, timeListOutputIndex);
            timeListOutputIndex += 1;
          }
        }
      }
      isTimerTicking = false;
      isUserViewingTimeList = !isUserViewingTimeList;
    } else if ( pressedButton == 'B'){
      if(isUserViewingTimeList){
        for(int i = 0; i < 2; i++){
          if(timeListOutputIndex < nextSavedTimeIndex){
            if(i == 0){
              lcd.clear();
            }
            LCD_WriteSavedTimePROGMEM(2, timeListOutputIndex);
            timeListOutputIndex += 1;
          }
        }
      }
    } else {
      if(isUserViewingTimeList){
        choosedNumber = choosedNumber * secondDigit + (int)(pressedButton) - 49;
        if(secondDigit == 10){
          choosedNumber += 10;
        }
        secondDigit = 10;
      }
    }
    digitalWrite(buzzerPin, HIGH);
    delay(200);
    digitalWrite(buzzerPin, LOW);
  }
}