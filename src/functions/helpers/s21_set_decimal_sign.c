#include "../../s21_decimal.h"

void s21_set_decimal_sign(s21_decimal* value, int sign) {
  int mask = 0x7FFFFFFF;
  value->bits[3] &= mask;
  value->bits[3] |= (sign << 31);
}