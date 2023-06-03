#define Z 0x01
#define A 0x02
#define V 0x03
#define D 0x04
#define K 0x05
#define B 0x06
#define O 0x07
#define H 0x08
#define N 0x09
#define R 0x0A
#define I 0x0B
#define V 0x0C
#define C 0x0D
#define Y 0x0E
#define _ 0x00

bool isAddress = true;
byte address;
bool isCommand = false;
byte command;
byte mistake;
uint8_t crc8_darc_table[256];

void setWriteModeRS485() {
  PORTD |= 1 << PD2;
  delay(1);
}

ISR(USART_TX_vect) {
  PORTD &= ~(1 << PD2);
}

int writeData() {
  byte message[26] = {
    Z,
    A,
    V,
    A,
    D,
    K,
    A,
    _,
    B,
    O,
    H,
    D,
    A,
    N,
    _,
    A,
    N,
    D,
    R,
    I,
    I,
    O,
    V,
    Y,
    C,
    H,
  };
  for (int j = 0; j < 5; j++) {
    for (int i = 0; i < 26; i++) {
      if (j == 1 && i == 25) {
        mistake = message[i];
        Serial.write((0 << mistake));
        byte crc = calculate_crc(message[i]);
        Serial.write(crc);
      } else if (j == 2 && i == 4) {
        mistake = message[i];
        Serial.write((0 << mistake) | (2 << mistake) | (5 << mistake));
        byte crc = calculate_crc(message[i]);
        Serial.write(crc);
      } else {
        Serial.write(message[i]);
        byte crc = calculate_crc(message[i]);
        Serial.write(crc);
      }
    }
  }
}

void setup() {
  delay(1000);
  DDRD = 0b00000111;
  PORTD = 0b11111000;
  Serial.begin(38400, SERIAL_8N1);
  UCSR0B |= (1 << UCSZ02) | (1 << TXCIE0);
  UCSR0A |= (1 << MPCM0);
  delay(1);
  address = 0x17;
  generate_crc8_darc_table();
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
        writeData();
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