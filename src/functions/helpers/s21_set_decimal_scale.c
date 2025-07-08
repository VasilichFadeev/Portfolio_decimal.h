#include "../../s21_decimal.h"

void s21_set_decimal_scale(s21_decimal* value, int scale) {
  scale <<= 16;
  unsigned int mask = 0xFF00FFFF;
  value->bits[3] &= mask;
  value->bits[3] |= scale;
}