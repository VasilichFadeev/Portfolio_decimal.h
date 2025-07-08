#include <string.h>

#include "../../s21_decimal.h"

static int s21_reduce_and_round(uint32_t bits[4], int *scale) {
  while (*scale > 0) {
    uint64_t rem = 0;
    uint32_t q[4] = {0};

    for (int i = 3; i >= 0; --i) {
      uint64_t cur = (rem << 32) | bits[i];
      q[i] = (uint32_t)(cur / 10);
      rem = cur % 10;
    }

    if (rem > 5 || (rem == 5 && (q[0] & 1))) {
      for (int i = 0; i < 4; ++i) {
        uint64_t sum = (uint64_t)q[i] + 1;
        q[i] = (uint32_t)sum;
        if ((sum >> 32) == 0) break;
      }
    }

    (*scale)--;

    if (q[3] == 0) {
      bits[0] = q[0];
      bits[1] = q[1];
      bits[2] = q[2];
      bits[3] = 0;
      return 0;
    }

    for (int i = 0; i < 4; ++i) bits[i] = q[i];
  }

  return 3;
}

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int status = 0;
  s21_clean_decimal(result);

  s21_normalize_decimals(&value_1, &value_2);
  int scale = s21_get_scale_for_round(value_1);
  s21_set_decimal_scale(result, scale);

  int sign_1 = s21_get_decimal_sign(value_1);
  int sign_2 = s21_get_decimal_sign(value_2);

  if (sign_1 == sign_2) {
    uint32_t temp[4] = {0};
    uint32_t carry = 0;

    for (int i = 0; i < 3; ++i) {
      uint64_t sum =
          (uint64_t)value_1.bits[i] + (uint64_t)value_2.bits[i] + carry;
      temp[i] = (uint32_t)sum;
      carry = (uint32_t)(sum >> 32);
    }

    temp[3] = carry;

    if (temp[3] == 0) {
      result->bits[0] = temp[0];
      result->bits[1] = temp[1];
      result->bits[2] = temp[2];
      s21_set_decimal_sign(result, sign_1);
    } else {
      if (s21_reduce_and_round(temp, &scale) == 0) {
        result->bits[0] = temp[0];
        result->bits[1] = temp[1];
        result->bits[2] = temp[2];
        s21_set_decimal_sign(result, sign_1);
        s21_set_decimal_scale(result, scale);
      } else {
        s21_clean_decimal(result);
        status = (sign_1 ? 2 : 1);
      }
    }
  } else {
    s21_decimal tmp = value_2;
    s21_set_decimal_sign(&tmp, !sign_2);
    status = s21_sub(value_1, tmp, result);
  }

  return status;
}