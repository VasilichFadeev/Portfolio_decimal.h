#include "../../s21_decimal.h"

void s21_abs_decimal(s21_decimal value, s21_decimal* result) {
  *result = value;
  s21_set_decimal_sign(result, 0);
}