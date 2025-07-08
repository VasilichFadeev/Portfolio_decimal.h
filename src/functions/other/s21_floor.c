#include "../../s21_decimal.h"

int s21_floor(s21_decimal value, s21_decimal *result) {
  if (!result) return 1;

  s21_decimal truncated = {0};
  s21_truncate(value, &truncated);

  if (s21_get_sign(value) && !s21_is_equal(value, truncated)) {
    s21_decimal one = {{1, 0, 0, 0}};
    s21_sub(truncated, one, result);
    s21_set_sign(result, 1);
  } else {
    *result = truncated;
  }

  return 0;
}