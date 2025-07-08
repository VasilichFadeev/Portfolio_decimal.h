#include "../../s21_decimal.h"

#define S21_DECIMAL_OK 0
#define S21_DECIMAL_ERROR 1

int s21_shift_left_for_div(s21_decimal* value, int shift) {
  for (int s = 0; s < shift; s++) {
    unsigned int overflow = 0;
    for (int i = 0; i < 3; i++) {
      unsigned int new_overflow = value->bits[i] >> 31;
      value->bits[i] = (value->bits[i] << 1) | overflow;
      overflow = new_overflow;
    }
    if (overflow) {
      return S21_DECIMAL_ERROR;
    }
  }
  return S21_DECIMAL_OK;
}