#include <Wire.h>
#define RTC_ADDRESS 0x51
byte second, minute, hour, day, month, year;

bool isAddress = true;
byte address;
bool isCommand = false;
byte command;
uint8_t crc8_darc_table[256];



void readData() {
  Wire.beginTransmission(RTC_ADDRESS);
  Wire.write(0x02);
  Wire.endTransmission();
  Wire.requestFrom(RTC_ADDRESS, 3);
  if (Wire.available()) {
    second = Wire.read() & 0x7F;
    minute = Wire.read() & 0x7F;
    hour = Wire.read() & 0x3F;
  }

  Wire.beginTransmission(RTC_ADDRESS);
  Wire.write(0x05);
  Wire.endTransmission();
  Wire.requestFrom(RTC_ADDRESS, 1);
  if (Wire.available()) {
    day = Wire.read() & 0x3F;
  }

  Wire.beginTransmission(RTC_ADDRESS);
  Wire.write(0x06);
  Wire.endTransmission();
  Wire.requestFrom(RTC_ADDRESS, 1);
  if (Wire.available()) {
    month = Wire.read() & 0x1F;
  }

  Wire.beginTransmission(RTC_ADDRESS);
  Wire.write(0x07);
  Wire.endTransmission(false);
  Wire.requestFrom(RTC_ADDRESS, 1);
  if (Wire.available()) {
    year = Wire.read();
  }

  Serial.write(second);
  Serial.write(calculate_crc(second));
  Serial.write(minute);
  Serial.write(calculate_crc(minute));
  Serial.write(hour);
  Serial.write(calculate_crc(hour));
  Serial.write(day);
  Serial.write(calculate_crc(day));
  Serial.write(month);
  Serial.write(calculate_crc(month));
  Serial.write(year);
  Serial.write(calculate_crc(year));
}

void setWriteModeRS485() {
  PORTD |= 1 << PD2;
  delay(1);
}

ISR(USART_TX_vect) {
  PORTD &= ~(1 << PD2);
}

void setup() {
  delay(1000);
  DDRD = 0b00000111;
  PORTD = 0b11111000;
  Serial.begin(9600, SERIAL_8N1);
  UCSR0B |= (1 << UCSZ02) | (1 << TXCIE0);
  UCSR0A |= (1 << MPCM0);
  delay(1);
  address = 0x36;
  generate_crc8_darc_table();

  Wire.begin();
  Wire.beginTransmission(RTC_ADDRESS);
  Wire.write(0x07);
  Wire.write(0x23);
  Wire.endTransmission();
}

void loop() {
  if (Serial.available()) {
    byte inByte = Serial.read();
    if (isAddress) {
      if (address == inByte) {
        isAddress = false;
        isCommand = true;
        UCSR0A &= ~(1 << MPCM0);
      }
    } else if (isCommand) {
      command = inByte;
      isCommand = false;
      if (command = 0xC1) {
        isAddress = true;
        setWriteModeRS485();
        readData();
      }
    }
  }
}

void generate_crc8_darc_table() {
  for (int i = 0; i < 256; i++) {
    byte crc = i;
    for (int j = 0; j < 8; j++) {
      if ((crc & 0x80) != 0) {
        crc = (byte)((crc << 1) ^ 0x39);
      } else {
        crc = (byte)(crc << 1);
      }
    }
    crc8_darc_table[i] = crc;
  }
}

uint8_t calculate_crc(const byte message) {
  uint8_t crc = 0;
  crc = crc8_darc_table[crc ^ message];
  return crc;
}