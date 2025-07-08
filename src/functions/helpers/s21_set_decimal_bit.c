#include "../../s21_decimal.h"

void set_bit(unsigned int* word, int index, int value) {
  int mask = 1 << index;
  *word &= ~mask;
  value <<= index;
  *word |= value;
}

void s21_set_decimal_bit(s21_decimal* decimal, int index, int value) {
  if (index >= 0 && index <= 31) {
    set_bit(&decimal->bits[0], index, value);
  } else if (index >= 32 && index <= 63) {
    set_bit(&decimal->bits[1], index, value);
  } else if (index >= 64 && index <= 95) {
    set_bit(&decimal->bits[2], index, value);
  }
}