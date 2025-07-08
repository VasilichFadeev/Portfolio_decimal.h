#include "../../s21_decimal.h"

void s21_normalize(s21_decimal *value_1, s21_decimal *value_2) {
  int scale1 = (value_1->bits[3] >> 16) & 0xFF;
  int scale2 = (value_2->bits[3] >> 16) & 0xFF;

  while (scale1 < scale2) {
    s21_mul_by_10(value_1);
    scale1++;
  }

  while (scale2 < scale1) {
    s21_mul_by_10(value_2);
    scale2++;
  }

  value_1->bits[3] = (value_1->bits[3] & 0x80000000) | (scale1 << 16);
  value_2->bits[3] = (value_2->bits[3] & 0x80000000) | (scale2 << 16);
}