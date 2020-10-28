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

#define bits 3u
#define loop_tick_time 150
#define tick_time 300
item output[1u << bits];
c_item input[bits];

void tick() {
  uint8_t res = 0;
  for (uint8_t i = 0; i < bits; i++) {
    if (!c_get(input[i])) {
      res += (1u << i);
    }
  }
  PORTD = 0x00;
  set(output[res], 1);
}

void loop() {
  static int counter = 0;
  if (counter >= tick_time)
    tick(), counter = 0;
  _delay_ms(loop_tick_time);
  counter += loop_tick_time;
}

void init() {
  DDRB = 0x00;
  for (uint8_t i = 0; i < bits; i++) {
    PORTB += (1u << i);
    input[i] = make_c_item(&PINB, i);
  }

  DDRD = 0x00;

  for (uint8_t i = 0; i < (1u << bits); i++) {
    DDRD += (1u << i);
    output[i] = make_item(&PORTD, i);
  }
}

int main() {
  init();
  while (1) loop();

  return 0;
}
