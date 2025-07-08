#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../s21_decimal.h"

static int handle_special_cases(float src, s21_decimal *dst) {
  if (!dst || isnan(src) || isinf(src)) {
    return 1;
  }

  s21_clean_decimal(dst);

  if (src == 0.0f) {
    return 0;
  }

  return -1;
}

static int check_value_range(float abs_src) {
  const float MIN_NORM = 1e-28f;
  const float MAX_DEC = 79228162514264337593543950335.0f;

  if ((abs_src > 0.0f && abs_src < MIN_NORM) || abs_src > MAX_DEC) {
    return 1;
  }
  return 0;
}

static void process_sign(float *src, int *sign) {
  if (*src < 0.0f) {
    *sign = 1;
    *src = -*src;
  }
}

static void extract_mantissa_and_exponent(float src, char *mantissa,
                                          int *exp10) {
  char tmp[32];
  snprintf(tmp, sizeof(tmp), "%.7g", src);

  char *e_pos = strchr(tmp, 'e');
  if (!e_pos) e_pos = strchr(tmp, 'E');

  if (e_pos) {
    *e_pos = '\0';
    *exp10 = atoi(e_pos + 1);
  }
  strcpy(mantissa, tmp);
}

static void process_mantissa(char *mantissa, int exp10, char *digits,
                             int *mant_len, int *frac_len) {
  char *dot = strchr(mantissa, '.');
  if (dot) {
    *frac_len = strlen(dot + 1);
    memmove(dot, dot + 1, *frac_len + 1);
  }
  *mant_len = strlen(mantissa);

  int new_frac = *frac_len - exp10;

  if (new_frac < 0) {
    strcpy(digits, mantissa);
    for (int i = 0; i < -new_frac; ++i) {
      digits[*mant_len + i] = '0';
    }
    digits[*mant_len - new_frac] = '\0';
    *frac_len = 0;
    *mant_len = strlen(digits);
  } else {
    if (exp10 < 0) {
      int pad = -exp10;
      memset(digits, '0', pad);
      strcpy(digits + pad, mantissa);
      *mant_len += pad;
    } else {
      strcpy(digits, mantissa);
    }
    *frac_len = new_frac;
    *mant_len = strlen(digits);
  }
}

static void normalize_digits(char *digits, int *mant_len, int *frac_len) {
  char *p = digits;
  while (*p == '0' && *(p + 1) && *(p + 1) != '\0' &&
         *mant_len > *frac_len + 1) {
    ++p;
    --*mant_len;
  }
  memmove(digits, p, *mant_len + 1);

  while (*frac_len > 0 && digits[*mant_len - 1] == '0') {
    digits[--*mant_len] = '\0';
    --*frac_len;
  }
}

static void adjust_precision(char *digits, int *mant_len, int *frac_len) {
  if (*frac_len > 28) {
    int excess = *frac_len - 28;
    int round_pos = *mant_len - excess;

    if (round_pos < *mant_len && digits[round_pos] >= '5') {
      digits[round_pos] = '\0';
      *mant_len = round_pos;

      int carry = 1;
      for (int i = *mant_len - 1; i >= 0 && carry; i--) {
        int digit = digits[i] - '0' + carry;
        if (digit > 9) {
          digits[i] = '0';
          carry = 1;
        } else {
          digits[i] = '0' + digit;
          carry = 0;
        }
      }

      if (carry) {
        memmove(digits + 1, digits, *mant_len + 1);
        digits[0] = '1';
        (*mant_len)++;
        if (*frac_len > 0) {
          (*frac_len)++;
          if (*frac_len > 28) {
            digits[--*mant_len] = '\0';
            *frac_len = 28;
          }
        }
      }
    } else {
      *mant_len -= excess;
      digits[*mant_len] = '\0';
    }

    *frac_len = 28;
  }
}

static int convert_digits_to_decimal(const char *digits, int mant_len,
                                     s21_decimal *dst) {
  for (int i = 0; i < mant_len; ++i) {
    if (!isdigit(digits[i])) continue;
    int dig = digits[i] - '0';

    uint64_t carry = 0;
    for (int w = 0; w < 3; ++w) {
      uint64_t prod = (uint64_t)dst->bits[w] * 10 + carry;
      dst->bits[w] = (uint32_t)(prod & 0xFFFFFFFF);
      carry = prod >> 32;
    }
    if (carry) return 1;

    uint64_t sum = (uint64_t)dst->bits[0] + dig;
    dst->bits[0] = (uint32_t)(sum & 0xFFFFFFFF);
    carry = sum >> 32;

    for (int w = 1; carry && w < 3; ++w) {
      uint64_t s = (uint64_t)dst->bits[w] + carry;
      dst->bits[w] = (uint32_t)(s & 0xFFFFFFFF);
      carry = s >> 32;
    }
    if (carry) return 1;
  }
  return 0;
}

int s21_from_float_to_decimal(float src, s21_decimal *dst) {
  int result = handle_special_cases(src, dst);
  if (result != -1) {
    return result;
  }

  float abs_src = src < 0.0f ? -src : src;
  if (check_value_range(abs_src)) {
    return 1;
  }

  int sign = 0;
  process_sign(&src, &sign);

  char mantissa[32] = {0};
  int exp10 = 0;
  extract_mantissa_and_exponent(src, mantissa, &exp10);

  char digits[64] = {0};
  int mant_len = 0;
  int frac_len = 0;
  process_mantissa(mantissa, exp10, digits, &mant_len, &frac_len);

  normalize_digits(digits, &mant_len, &frac_len);
  adjust_precision(digits, &mant_len, &frac_len);

  if (convert_digits_to_decimal(digits, mant_len, dst)) {
    return 1;
  }

  s21_set_decimal_scale(dst, frac_len);
  s21_set_decimal_sign(dst, sign);

  return 0;
}