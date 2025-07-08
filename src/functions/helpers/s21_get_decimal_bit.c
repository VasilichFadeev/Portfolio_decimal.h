#include "../../s21_decimal.h"

int get_bit(unsigned int value, int index) {
  int result = 0;
  int mask = 1 << index;
  result = (value & mask) >> index;
  return result;
}

int s21_get_decimal_bit(s21_decimal value, int index) {
  int bit = -1;

  if (index >= 0 && index <= 31) {
    bit = get_bit(value.bits[0], index);
  } else if (index >= 32 && index <= 63) {
    bit = get_bit(value.bits[1], index % 32);
  } else if (index >= 64 && index <= 95) {
    bit = get_bit(value.bits[2], index % 32);
  }
  return bit;
}