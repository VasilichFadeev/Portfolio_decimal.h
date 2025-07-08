#include "../../s21_decimal.h"

int s21_mul_by_10(s21_decimal *value) {
  s21_decimal original = *value;
  s21_decimal temp = original;
  s21_decimal temp2 = original;
  int flag = 1;

  // Проверка умножения на 8
  for (int i = 0; i < 3; i++) {
    if ((temp.bits[i] << 3) >> 3 != temp.bits[i]) {
      flag = 0;
      break;
    }
  }
  if (flag) {
    for (int i = 0; i < 3; i++) {
      temp.bits[i] <<= 3;
    }
  }

  // Проверка умножения на 2
  if (flag) {
    for (int i = 0; i < 3; i++) {
      if ((temp2.bits[i] << 1) >> 1 != temp2.bits[i]) {
        flag = 0;
        break;
      }
    }
    if (flag) {
      for (int i = 0; i < 3; i++) {
        temp2.bits[i] <<= 1;
      }
    }
  }

  // Сложение результатов
  s21_decimal sum = {0};
  if (flag && s21_add(temp, temp2, &sum)) {
    flag = 0;
  }

  if (flag) {
    *value = sum;
  } else {
    *value = original;
  }

  return flag;
}