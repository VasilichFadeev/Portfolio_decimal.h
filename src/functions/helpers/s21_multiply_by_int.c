#include "../../s21_decimal.h"

int s21_multiply_by_int(s21_decimal* value, int multiplier) {
  unsigned long long temp_result[4] = {0};
  for (int i = 0; i < 3; i++) {
    unsigned long long mul = (unsigned long long)value->bits[i] * multiplier;
    temp_result[i] += mul;
    if (temp_result[i] > 0xFFFFFFFF) {
      temp_result[i + 1] += temp_result[i] >> 32;
      temp_result[i] &= 0xFFFFFFFF;
    }
  }
  if (temp_result[3]) {
    return 3;
  }
  value->bits[0] = (unsigned int)temp_result[0];
  value->bits[1] = (unsigned int)temp_result[1];
  value->bits[2] = (unsigned int)temp_result[2];
  return 0;
}