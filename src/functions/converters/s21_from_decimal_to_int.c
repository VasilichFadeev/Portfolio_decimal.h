#include <limits.h>

#include "../../s21_decimal.h"

int s21_from_decimal_to_int(s21_decimal src, int *dst) {
  int flag = 0;

  if (!dst) {
    flag = 1;
  } else {
    s21_decimal truncated = {0};
    s21_truncate(src, &truncated);

    if (truncated.bits[1] || truncated.bits[2]) {
      flag = 1;
    } else {
      uint32_t num = truncated.bits[0];
      int sign = s21_get_sign(truncated);

      // Округление если есть дробная часть
      if (s21_get_scale(&src) > 0) {
        s21_decimal rem = {0};
        s21_mod(src, (s21_decimal){{10, 0, 0, 0}}, &rem);
        if (rem.bits[0] >= 5) num++;
      }

      // Проверка переполнения и установка значения
      if (sign) {
        if (num > (uint32_t)INT_MAX + 1) {
          flag = 1;
        } else {
          *dst = (num == (uint32_t)INT_MAX + 1) ? INT_MIN : -(int)num;
        }
      } else {
        if (num > INT_MAX) {
          flag = 1;
        } else {
          *dst = (int)num;
        }
      }
    }
  }

  return flag;
}