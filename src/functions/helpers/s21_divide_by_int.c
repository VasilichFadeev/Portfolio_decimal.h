#include "../../s21_decimal.h"

unsigned int s21_divide_by_int(s21_decimal* dec, int divisor) {
  unsigned long long rem = 0;
  for (int i = 2; i >= 0; i--) {
    unsigned long long current = (rem << 32) | dec->bits[i];
    dec->bits[i] = (unsigned int)(current / divisor);
    rem = current % divisor;
  }
  return rem;
}