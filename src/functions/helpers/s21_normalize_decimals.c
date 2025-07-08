#include "../../s21_decimal.h"

void s21_normalize_decimals(s21_decimal* value_1, s21_decimal* value_2) {
  int scale_1 = s21_get_scale_for_round(*value_1);
  int scale_2 = s21_get_scale_for_round(*value_2);
  int diff;
  if (scale_1 < scale_2) {
    diff = scale_2 - scale_1;
    for (int i = 0; i < diff; i++) {
      if (s21_multiply_by_int(value_1, 10) == 1) {
        return;
      }
    }
    s21_set_decimal_scale(value_1, scale_2);
  } else if (scale_2 < scale_1) {
    diff = scale_1 - scale_2;
    for (int i = 0; i < diff; i++) {
      if (s21_multiply_by_int(value_2, 10) == 1) {
        return;
      }
    }
    s21_set_decimal_scale(value_2, scale_1);
  }
}