#include "../../s21_decimal.h"

int s21_is_divisible_by_10(const s21_decimal dec) {
  s21_decimal temp = dec;
  unsigned int rem = s21_divide_by_int(&temp, 10);
  return (rem == 0);
}