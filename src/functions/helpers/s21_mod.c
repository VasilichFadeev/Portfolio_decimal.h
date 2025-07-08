#include "../../s21_decimal.h"

#define S21_DECIMAL_OK 0
#define S21_DECIMAL_ERROR 1
#define S21_DECIMAL_SMALL 2
#define S21_DECIMAL_ZERO_DIV 3

int s21_mod(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int flag = S21_DECIMAL_OK;
  if (!result) flag = S21_DECIMAL_ERROR;
  if (s21_is_zero(&value_2)) flag = S21_DECIMAL_ZERO_DIV;

  int sign = s21_get_sign(value_1);
  s21_set_sign(&value_1, 0);
  s21_set_sign(&value_2, 0);

  s21_decimal quotient = {0};
  s21_div(value_1, value_2, &quotient);
  s21_truncate(quotient, &quotient);

  s21_decimal temp = {0};
  s21_mul(quotient, value_2, &temp);
  s21_sub(value_1, temp, result);

  s21_set_sign(result, sign);

  if (s21_is_zero(result)) {
    s21_set_sign(result, 0);
  }

  return flag;
}