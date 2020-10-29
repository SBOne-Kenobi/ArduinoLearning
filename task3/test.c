#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

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
c_item interrupt_button;

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
  _delay_us(10);
  set(SH_CP, 0);
  _delay_us(10);
}

void sendPack(uint8_t n) {
  for (uint8_t i = 0; i < 8; i++) {
    sendData(n & 1u);
    n >>= 1u;
  }
  set(ST_CP, 1);
  _delay_us(20);
  set(ST_CP, 0);
}

void init() {
  DDRD = 0b00111000;
  DS = make_item(&PORTD, 5);
  ST_CP = make_item(&PORTD, 4);
  SH_CP = make_item(&PORTD, 3);
  PORTD |= (1u << 2u);
  interrupt_button = make_c_item(&PIND, 2);

  set(SH_CP, 0);
  set(ST_CP, 0);
  sendPack(digits[0]);

  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  OCR1A = 15624;
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS10);
  TCCR1B |= (1 << CS12);
  TIMSK1 |= (1 << OCIE1A);

  EICRA |= (1 << ISC01);
  EIMSK |= (1 << INT0);

  sei();
}

volatile uint8_t cur_time = 0;

ISR(TIMER1_COMPA_vect) {
  cur_time = (cur_time + 1) % 10;
  sendPack(digits[cur_time]);
}

ISR(INT0_vect) {
  cur_time = 0;
  sendPack(digits[0]);
  TCNT1 = 0;
}

int main() {
  init();
  while (1);
  return 0;
}
