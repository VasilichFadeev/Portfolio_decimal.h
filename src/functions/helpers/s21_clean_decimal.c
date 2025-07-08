#include "../../s21_decimal.h"

void s21_clean_decimal(s21_decimal* dec) {
  for (int i = 0; i < 4; i++) {
    dec->bits[i] = 0;
  }
}