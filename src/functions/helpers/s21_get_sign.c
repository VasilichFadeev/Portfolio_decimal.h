#include "../../s21_decimal.h"

int s21_get_sign(s21_decimal value) { return (value.bits[3] >> 31) & 1; }