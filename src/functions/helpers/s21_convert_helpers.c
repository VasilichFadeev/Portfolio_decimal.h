#include <math.h>
#include <string.h>

#include "../../s21_decimal.h"

int s21_get_scale(s21_decimal *d) { return (d->bits[3] >> 16) & 0xFF; }

void s21_set_scale(s21_decimal *d, int scale) {
  d->bits[3] &= 0x80000000;
  d->bits[3] |= (scale << 16);
}

void s21_set_sign(s21_decimal *value, int sign) {
  if (sign) {
    value->bits[3] |= 0x80000000;
  } else {
    value->bits[3] &= ~0x80000000;
  }
}

void s21_init_decimal(s21_decimal *d) {
  for (int i = 0; i < 4; i++) d->bits[i] = 0;
}
