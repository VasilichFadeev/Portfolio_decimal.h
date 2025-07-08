#include <math.h>

#include "../../s21_decimal.h"

int s21_from_decimal_to_float(s21_decimal src, float *dst) {
  if (!dst) return 1;

  long double result = 0.0;
  int scale = s21_get_scale(&src);

  result += (long double)src.bits[0];
  result += (long double)src.bits[1] * 4294967296.0;
  result += (long double)src.bits[2] * 18446744073709551616.0;

  if (scale > 0) {
    long double power = 1.0;
    for (int i = 0; i < scale; i++) power *= 10.0;
    result /= power;
  }

  if (s21_get_sign(src)) result = -result;

  *dst = (float)result;
  return (isinf(*dst) || isnan(*dst)) ? 1 : 0;
}