#include "../../s21_decimal.h"

#define S21_DECIMAL_OK 0
#define S21_DECIMAL_ERROR 1
#define S21_DECIMAL_SMALL 2
#define S21_DECIMAL_ZERO_DIV 3

int s21_reduce_and_round_s(uint32_t temp[4], int *scale) {
  if (*scale == 0) return S21_DECIMAL_SMALL;

  uint64_t remainder = 0;
  for (int i = 2; i >= 0; i--) {
    uint64_t cur = ((uint64_t)remainder << 32) | temp[i];
    temp[i] = (uint32_t)(cur / 10);
    remainder = cur % 10;
  }

  if (remainder > 5 || (remainder == 5 && (temp[0] & 1))) {
    uint64_t carry = 1;
    for (int i = 0; i < 3 && carry; i++) {
      uint64_t sum = (uint64_t)temp[i] + carry;
      temp[i] = (uint32_t)(sum & 0xFFFFFFFF);
      carry = sum >> 32;
    }
    if (carry) return S21_DECIMAL_SMALL;
  }

  (*scale)--;
  return S21_DECIMAL_OK;
}

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int status = S21_DECIMAL_OK;
  s21_clean_decimal(result);

  s21_normalize_decimals(&value_1, &value_2);
  int scale = s21_get_scale_for_round(value_1);
  s21_set_decimal_scale(result, scale);

  int sign_1 = s21_get_decimal_sign(value_1);
  int sign_2 = s21_get_decimal_sign(value_2);

  if (sign_1 == sign_2) {
    s21_decimal abs1 = value_1, abs2 = value_2;
    s21_set_decimal_sign(&abs1, 0);
    s21_set_decimal_sign(&abs2, 0);

    int cmp = s21_compare_abs_3words(abs1, abs2);
    if (cmp == 0) {
      s21_clean_decimal(result);
    } else {
      s21_decimal bigger = abs1, smaller = abs2;
      int final_sign = sign_1;

      if (cmp < 0) {
        bigger = abs2;
        smaller = abs1;
        final_sign = !sign_1;
      }

      uint32_t temp[4] = {0};
      int borrow = 0;

      for (int i = 0; i < 3; ++i) {
        int64_t diff =
            (int64_t)bigger.bits[i] - (int64_t)smaller.bits[i] - borrow;
        if (diff < 0) {
          diff += ((int64_t)1 << 32);
          borrow = 1;
        } else {
          borrow = 0;
        }
        temp[i] = (uint32_t)diff;
      }

      temp[3] = 0;
      while (temp[3] != 0) {
        if (s21_reduce_and_round_s(temp, &scale) == S21_DECIMAL_OK) {
          continue;
        } else {
          s21_clean_decimal(result);
          return S21_DECIMAL_SMALL;
        }
      }

      result->bits[0] = temp[0];
      result->bits[1] = temp[1];
      result->bits[2] = temp[2];
      s21_set_decimal_sign(result, final_sign);
      s21_set_decimal_scale(result, scale);
    }
  } else {
    s21_decimal tmp_value_2 = value_2;
    s21_set_decimal_sign(&tmp_value_2, !sign_2);
    int add_result = s21_add(value_1, tmp_value_2, result);

    if (add_result == S21_DECIMAL_ERROR) {
      status =
          s21_get_decimal_sign(value_1) ? S21_DECIMAL_SMALL : S21_DECIMAL_ERROR;
      s21_clean_decimal(result);
    } else if (add_result == S21_DECIMAL_SMALL) {
      status =
          s21_get_decimal_sign(value_1) ? S21_DECIMAL_SMALL : S21_DECIMAL_ERROR;
      s21_clean_decimal(result);
    } else {
      s21_set_decimal_sign(result, s21_get_decimal_sign(*result));
    }
  }
  return status;
}
