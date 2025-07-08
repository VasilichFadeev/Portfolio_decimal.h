#include "../../s21_decimal.h"

int s21_compare_abs_3words(s21_decimal val1, s21_decimal val2) {
  int result = 0;
  int i = 2;
  int flag = 0;

  while (i >= 0 && flag == 0) {
    if (val1.bits[i] != val2.bits[i]) {
      result = (val1.bits[i] > val2.bits[i]) ? 1 : -1;
      flag = 1;
    }
    i--;
  }
  return result;
}