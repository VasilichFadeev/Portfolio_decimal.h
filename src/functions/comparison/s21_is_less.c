#include "../../s21_decimal.h"

int s21_is_less(s21_decimal value_1, s21_decimal value_2) {
  int flag = 0;
  int comparison_done = 0;  // Флаг завершения сравнения

  if (s21_get_sign(value_1) != s21_get_sign(value_2)) {
    // Если знаки разные, сравниваем знаки
    flag = s21_get_sign(value_1) > s21_get_sign(value_2);
    comparison_done = 1;
  } else {
    // Нормализация перед сравнением
    s21_normalize(&value_1, &value_2);
    int sign = s21_get_sign(value_1);

    // Сравнение побитово от старших к младшим
    for (int i = 2; i >= 0 && !comparison_done; i--) {
      if (value_1.bits[i] != value_2.bits[i]) {
        flag = sign ? (value_1.bits[i] > value_2.bits[i])
                    : (value_1.bits[i] < value_2.bits[i]);
        comparison_done = 1;  // Устанавливаем флаг завершения сравнения
      }
    }
  }

  return flag;
}