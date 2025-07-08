#include <limits.h>
#include <string.h>

#include "../../s21_decimal.h"

#define S21_DECIMAL_OK 0
#define S21_DECIMAL_ERROR 1
#define S21_DECIMAL_SMALL 2
#define S21_DECIMAL_ZERO_DIV 3

static void s21_round_divide_by_10(uint32_t product[6]) {
  uint64_t rem = 0;
  uint32_t q[6] = {0};

  for (int i = 5; i >= 0; --i) {
    uint64_t cur = (rem << 32) | product[i];
    q[i] = (uint32_t)(cur / 10);
    rem = cur % 10;
  }

  if (rem > 5 || (rem == 5 && (q[0] & 1))) {
    uint64_t carry = 1;
    for (int i = 0; i < 6 && carry; ++i) {
      uint64_t s = (uint64_t)q[i] + carry;
      q[i] = (uint32_t)s;
      carry = s >> 32;
    }
  }

  for (int i = 0; i < 6; ++i) {
    product[i] = q[i];
  }
}

static int s21_fits_in_96_bits(uint32_t product[6]) {
  return product[3] == 0 && product[4] == 0 && product[5] == 0;
}

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  s21_clean_decimal(result);

  if (!s21_is_decimal_correct(value_1) || !s21_is_decimal_correct(value_2)) {
    return S21_DECIMAL_ERROR;
  }

  s21_decimal abs1, abs2;
  s21_abs_decimal(value_1, &abs1);
  s21_abs_decimal(value_2, &abs2);

  uint32_t product[6] = {0};

  for (int i = 0; i < 96; ++i) {
    if (s21_get_decimal_bit(abs2, i)) {
      uint32_t carry = 0;
      for (int j = 0; j < 3; ++j) {
        uint64_t shifted = (uint64_t)abs1.bits[j] << (i % 32);
        int word = (i / 32) + j;

        uint64_t sum = (uint64_t)product[word] + (uint32_t)shifted + carry;
        product[word] = (uint32_t)sum;
        carry = (uint32_t)(sum >> 32);

        int w = word + 1;
        if (w < 6) {
          uint64_t top = (uint64_t)shifted >> 32;
          sum = (uint64_t)product[w] + top + carry;
          product[w] = (uint32_t)sum;
          carry = (uint32_t)(sum >> 32);

          while (carry && ++w < 6) {
            sum = (uint64_t)product[w] + carry;
            product[w] = (uint32_t)sum;
            carry = (uint32_t)(sum >> 32);
          }

          if (carry) {
            int overflow_sign =
                s21_get_decimal_sign(value_1) ^ s21_get_decimal_sign(value_2);
            return overflow_sign
                       ? 2
                       : 1;  // 0 = положительное (1), 1 = отрицательное (2)
          }
        }
      }
    }
  }

  int scale1 = s21_get_scale_for_round(value_1);
  int scale2 = s21_get_scale_for_round(value_2);
  int current_scale = scale1 + scale2;

  int result_sign =
      s21_get_decimal_sign(value_1) ^ s21_get_decimal_sign(value_2);

  while (!s21_fits_in_96_bits(product) && current_scale > 0) {
    s21_round_divide_by_10(product);
    current_scale--;
  }

  while (current_scale > 28 && s21_fits_in_96_bits(product)) {
    s21_round_divide_by_10(product);
    current_scale--;
  }

  if (!s21_fits_in_96_bits(product)) {
    return result_sign ? 2 : 1;
  }

  result->bits[0] = product[0];
  result->bits[1] = product[1];
  result->bits[2] = product[2];
  s21_set_decimal_scale(result, current_scale);

  s21_set_decimal_sign(result, result_sign);

  return S21_DECIMAL_OK;
}