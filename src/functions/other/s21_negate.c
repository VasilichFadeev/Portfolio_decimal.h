#include "../../s21_decimal.h"

int s21_negate(s21_decimal value, s21_decimal *result) {
  int flag = 0;
  if (!result) flag = 1;

  s21_copy_decimal(&value, result);
  s21_set_sign(result, !s21_get_sign(value));

  return flag;
}