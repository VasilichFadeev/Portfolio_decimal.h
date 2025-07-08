#include "../../s21_decimal.h"

void s21_integer_division(s21_decimal dividend, s21_decimal divisor,
                          s21_decimal* quotient, s21_decimal* remainder) {
  s21_clean_decimal(quotient);
  s21_clean_decimal(remainder);
  for (int i = 95; i >= 0; i--) {
    s21_shift_left_for_div(remainder, 1);
    int bit = s21_get_decimal_bit(dividend, i);
    s21_set_decimal_bit(remainder, 0, bit);
    if (s21_compare_abs_3words(*remainder, divisor) >= 0) {
      s21_decimal temp;
      s21_sub(*remainder, divisor, &temp);
      *remainder = temp;
      s21_set_decimal_bit(quotient, i, 1);
    }
  }
}