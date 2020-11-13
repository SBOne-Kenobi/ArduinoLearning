#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


typedef volatile unsigned char T;

typedef struct item_impl {
  T _mask;
  T *_data;
} item;

typedef struct c_item_impl {
  T _mask;
  const T *_data;
} c_item;

item make_item(T *data, T port) {
  item res;
  res._data = data;
  res._mask = 1u << port;
  return res;
}

c_item make_c_item(const T *data, T port) {
  c_item res;
  res._data = data;
  res._mask = 1u << port;
  return res;
}

void set(item x, T r) {
  *x._data |= x._mask;
  if (!r)
    *x._data ^= x._mask;
}

T c_get(c_item x) {
  if (*x._data & x._mask)
    return 1;
  else
    return 0;
}

T get(item x) {
  if (*x._data & x._mask)
    return 1;
  else
    return 0;
}

item DS;
item ST_CP;
item SH_CP;

uint8_t digits[10] = {
        0b00111111,
        0b00000110,
        0b01011011,
        0b01001111,
        0b01100110,
        0b01101101,
        0b01111101,
        0b00000111,
        0b01111111,
        0b01101111,
};

void sendData(uint8_t x) {
  set(DS, x);
  set(SH_CP, 1);
  set(SH_CP, 0);
}

void sendPack(uint8_t n) {
  for (uint8_t i = 0; i < 8; i++) {
    sendData(n & 1u);
    n >>= 1u;
  }
  set(ST_CP, 1);
  set(ST_CP, 0);
}

void init_timers() {
  cli();
  EICRA |= (1 << ISC01);
  EIMSK |= (1 << INT0);

  EICRA |= (1 << ISC11);
  EIMSK |= (1 << INT1);
  sei();
}

void EEPROM_write(unsigned int uiAddress, unsigned char ucData) {
  while (EECR & (1 << EEPE));

  EEAR = uiAddress;
  EEDR = ucData;

  EECR |= (1 << EEMPE);
  EECR |= (1 << EEPE);
}

unsigned char EEPROM_read(unsigned int uiAddress) {
  while (EECR & (1 << EEPE));

  EEAR = uiAddress;
  EECR |= (1 << EERE);
  return EEDR;
}

void init_port() {
  DDRD = 0b01110000;
  DS = make_item(&PORTD, 6);
  ST_CP = make_item(&PORTD, 5);
  SH_CP = make_item(&PORTD, 4);
  PORTD |= (1u << 2u);
  PORTD |= (1u << 3u);

  set(SH_CP, 0);
  set(ST_CP, 0);
  sendPack(digits[0]);
}

volatile uint8_t counter = 0;

void init() {
  init_port();
  init_timers();

  counter = EEPROM_read(1);
}

ISR(INT1_vect) {
  counter = 0;
}

ISR(INT0_vect) {
  counter = (counter + 1) % 10;
}

void loop() {
  EEPROM_write(1, counter);
  sendPack(digits[counter]);
}

int main() {
  init();
  while (1) loop();
  return 0;
}
