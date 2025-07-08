#include "../../s21_decimal.h"

int s21_is_decimal_correct(s21_decimal value) {
  int result = 1;
  int sign = (value.bits[3] >> 31) & 1;
  if (sign != 0 && sign != 1) result = 0;
  int scale = (value.bits[3] >> 16) & 0x7F;
  if (scale < 0 || scale > 28) result = 0;
  if ((value.bits[3] & 0x0000FFFF) != 0) result = 0;
  if ((value.bits[3] & 0x7F000000) != 0) result = 0;
  return result;
}