#include "../../s21_decimal.h"

int s21_is_zero(const s21_decimal *dec) {
  return !dec->bits[0] && !dec->bits[1] && !dec->bits[2];
}