#include <string.h>

#include "../../s21_decimal.h"

int s21_is_equal(s21_decimal value_1, s21_decimal value_2) {
  int flag = 1;  // Предполагаем, что числа равны

  // Нормализуем числа
  s21_normalize(&value_1, &value_2);

  // Проверяем знаки
  int sign1 = s21_get_sign(value_1);
  int sign2 = s21_get_sign(value_2);

  if (sign1 != sign2) {
    // Если знаки разные, проверяем не являются ли оба нулями
    int is_zero1 = s21_is_zero(&value_1);
    int is_zero2 = s21_is_zero(&value_2);
    flag = is_zero1 && is_zero2;
  } else {
    // Проверяем биты мантиссы
    for (int i = 0; i < 3 && flag; i++) {
      if (value_1.bits[i] != value_2.bits[i]) {
        flag = 0;
      }
    }
  }

  return flag;
}