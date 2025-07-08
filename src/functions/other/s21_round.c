#include "../../s21_decimal.h"

#define S21_DECIMAL_OK 0
#define S21_DECIMAL_ERROR 1
#define S21_DECIMAL_SMALL 2
#define S21_DECIMAL_ZERO_DIV 3

int s21_round(s21_decimal value, s21_decimal *result) {
  if (result == NULL) return S21_DECIMAL_ERROR;

  int scale = s21_get_scale_for_round(value);
  if (scale == 0) {
    *result = value;
    return S21_DECIMAL_OK;
  }

  s21_decimal trunc;
  int status = s21_truncate(value, &trunc);
  if (status != S21_DECIMAL_OK) return status;

  s21_decimal frac;
  status = s21_sub(value, trunc, &frac);
  if (status != S21_DECIMAL_OK) return status;

  s21_decimal abs_frac;
  s21_abs_decimal(frac, &abs_frac);

  s21_decimal threshold;
  s21_clean_decimal(&threshold);
  threshold.bits[0] = 5;
  for (int i = 1; i < scale; i++) {
    status = s21_multiply_by_int(&threshold, 10);
    if (status != S21_DECIMAL_OK) return status;
  }
  s21_set_decimal_scale(&threshold, scale);

  if (s21_is_greater_or_equal(abs_frac, threshold)) {
    s21_decimal one;
    status = s21_from_int_to_decimal(1, &one);
    if (status != S21_DECIMAL_OK) return status;
    if (s21_get_decimal_sign(value) == 0) {
      status = s21_add(trunc, one, &trunc);
    } else {
      status = s21_sub(trunc, one, &trunc);
    }
    if (status != S21_DECIMAL_OK) return status;
  }

  s21_set_decimal_scale(&trunc, 0);
  *result = trunc;
  return S21_DECIMAL_OK;
}