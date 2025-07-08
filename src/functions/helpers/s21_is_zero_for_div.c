#include "../../s21_decimal.h"

#define S21_DECIMAL_OK 0
#define S21_DECIMAL_ZERO_DIV 3

int s21_is_zero_for_div(s21_decimal value) {
  int status = S21_DECIMAL_OK;
  if ((value.bits[0] == 0 && value.bits[1] == 0 && value.bits[2] == 0)) {
    status = S21_DECIMAL_ZERO_DIV;
  }
  return status;
}