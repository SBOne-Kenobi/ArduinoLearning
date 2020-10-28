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
#define tick_time 250
item output[bits];
c_item input[1u << bits];

void setOutput(unsigned char n) {
  for (int i = 0; i < bits; i++) {
    set(output[i], n & 1u);
    n >>= 1u;
  }
}

void tick() {
  for (int i = 0; i < (1u << bits); i++)
    if (!c_get(input[i])) {
      setOutput(i);
      break;
    }
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
    DDRB += (1u << i);
    output[i] = make_item(&PORTB, i);
  }

  DDRD = 0x00;

  for (uint8_t i = 0; i < (1u << bits); i++) {
    PORTD += (1u << i);
    input[i] = make_c_item(&PIND, i);
  }
}

int main() {
  init();
  while (1) loop();

  return 0;
}
