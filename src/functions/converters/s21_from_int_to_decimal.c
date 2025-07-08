#include <math.h>
#include <string.h>

#include "../../s21_decimal.h"

int s21_from_int_to_decimal(int src, s21_decimal *dst) {
  int flag = 0;  // По умолчанию успешное выполнение

  if (!dst) {
    flag = 1;  // Ошибка: неверный указатель
  } else {
    s21_init_decimal(dst);  // Инициализация decimal нулями

    if (src < 0) {
      s21_set_sign(dst, 1);  // Установка отрицательного знака
      src = -src;  // Работаем с абсолютным значением
    }

    dst->bits[0] = src;  // Запись значения в младший бит
  }

  return flag;
}