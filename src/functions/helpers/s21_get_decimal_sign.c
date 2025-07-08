#include "../../s21_decimal.h"

int s21_get_decimal_sign(s21_decimal value) {
  unsigned int result = 1;
  result <<= 31;
  result &= value.bits[3];
  result >>= 31;
  return result;
}