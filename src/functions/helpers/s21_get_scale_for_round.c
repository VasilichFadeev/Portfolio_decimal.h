#include "../../s21_decimal.h"

unsigned int s21_get_scale_for_round(s21_decimal value) {
  unsigned int scale = s21_get_second_byte(value.bits[3]);
  scale >>= 16;
  return scale;
}