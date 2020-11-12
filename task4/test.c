#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
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

void port_init() {
  DDRD = 0b00111000;
  DS = make_item(&PORTD, 5);
  ST_CP = make_item(&PORTD, 4);
  SH_CP = make_item(&PORTD, 3);

  set(SH_CP, 0);
  set(ST_CP, 0);
  sendPack(digits[0]);
}

void adc_init() {
  ADCSRA |= (1 << ADEN)
            | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
  ADMUX |= (1 << REFS1) | (1 << REFS0);
}

unsigned int adc_convert() {
  ADCSRA |= (1 << ADSC);
  while ((ADCSRA & (1 << ADSC)));
  return ADC;
}

void init() {
  port_init();
  adc_init();
}

void loop() {
  unsigned cur_value = (adc_convert() * 10) / 1024;
  sendPack(digits[cur_value]);
}

int main() {
  init();
  while (1) loop();
  return 0;
}
