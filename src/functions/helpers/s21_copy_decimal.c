#include "../../s21_decimal.h"

void s21_copy_decimal(const s21_decimal *src, s21_decimal *dst) {
  for (int i = 0; i < 4; i++) {
    dst->bits[i] = src->bits[i];
  }
}