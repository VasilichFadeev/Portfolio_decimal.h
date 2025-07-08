#include "../../s21_decimal.h"

int s21_get_second_byte(int value) {
  return value & 0b00000000111111110000000000000000;
}