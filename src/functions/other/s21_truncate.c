#include "../../s21_decimal.h"

int s21_truncate(s21_decimal value, s21_decimal *result) {
  int flag = 0;
  if (!result) flag = 1;

  int scale = s21_get_scale(&value);
  if (scale == 0) {
    *result = value;
    flag = 0;
  }

  // Делим на 10 scale раз
  *result = value;
  while (scale-- > 0) {
    // Реализация деления на 10
    uint64_t remainder = 0;
    for (int i = 2; i >= 0; i--) {
      uint64_t n = ((uint64_t)remainder << 32) | result->bits[i];
      result->bits[i] = n / 10;
      remainder = n % 10;
    }
  }

  s21_set_scale(result, 0);
  return flag;
}