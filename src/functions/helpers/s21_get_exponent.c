#include "../../s21_decimal.h"

int s21_get_exponent(s21_decimal value) { return (value.bits[3] >> 16) & 0xFF; }