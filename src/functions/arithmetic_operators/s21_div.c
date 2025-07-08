#include <string.h>

#include "../../s21_decimal.h"

#define S21_DECIMAL_OK 0
#define S21_DECIMAL_ERROR 1
#define S21_DECIMAL_SMALL 2
#define S21_DECIMAL_ZERO_DIV 3

void s21_round_divide_decimal_by_10(s21_decimal* value) {
  unsigned int remainder = s21_divide_by_int(value, 10);

  if (remainder > 5 || (remainder == 5 && (value->bits[0] & 1))) {
    s21_decimal one;
    s21_clean_decimal(&one);
    one.bits[0] = 1;
    s21_decimal temp;
    if (s21_add(*value, one, &temp) == S21_DECIMAL_OK) {
      *value = temp;
    }
  }
}

static int s21_is_decimal_overflow(const s21_decimal* value) {
  s21_decimal max_decimal;
  s21_clean_decimal(&max_decimal);
  max_decimal.bits[0] = 0xFFFFFFFF;
  max_decimal.bits[1] = 0xFFFFFFFF;
  max_decimal.bits[2] = 0xFFFFFFFF;

  return s21_compare_abs_3words(*value, max_decimal) > 0;
}

int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal* result) {
  if (s21_is_zero_for_div(value_2)) {
    s21_clean_decimal(result);
    return S21_DECIMAL_ZERO_DIV;
  }

  if (!result) {
    return S21_DECIMAL_ERROR;
  } else {
    s21_clean_decimal(result);
  }

  if (!s21_is_decimal_correct(value_1) || !s21_is_decimal_correct(value_2)) {
    return S21_DECIMAL_ERROR;
  }

  int result_sign =
      s21_get_decimal_sign(value_1) ^ s21_get_decimal_sign(value_2);

  s21_decimal abs_dividend, abs_divisor;
  s21_abs_decimal(value_1, &abs_dividend);
  s21_abs_decimal(value_2, &abs_divisor);

  int scale1 = s21_get_scale_for_round(value_1);
  int scale2 = s21_get_scale_for_round(value_2);

  s21_set_decimal_scale(&abs_dividend, 0);
  s21_set_decimal_scale(&abs_divisor, 0);

  int base_result_scale = scale1 - scale2;

  int precision_added = 0;
  const int max_precision = 28;

  while (precision_added < max_precision) {
    s21_decimal test_dividend = abs_dividend;
    if (s21_multiply_by_int(&test_dividend, 10) != S21_DECIMAL_OK) {
      break;
    }
    abs_dividend = test_dividend;
    precision_added++;
  }

  s21_decimal quotient, remainder;
  s21_integer_division(abs_dividend, abs_divisor, &quotient, &remainder);

  int result_scale = base_result_scale + precision_added;

  while (!s21_is_zero_for_div(remainder) && result_scale < 28) {
    s21_decimal test_quotient = quotient;
    if (s21_multiply_by_int(&test_quotient, 10) != S21_DECIMAL_OK) {
      break;
    }

    s21_decimal test_remainder = remainder;
    if (s21_multiply_by_int(&test_remainder, 10) != S21_DECIMAL_OK) {
      break;
    }

    s21_decimal extra_digit, new_remainder;
    s21_integer_division(test_remainder, abs_divisor, &extra_digit,
                         &new_remainder);

    s21_decimal temp_result;
    if (s21_add(test_quotient, extra_digit, &temp_result) != S21_DECIMAL_OK) {
      break;
    }

    quotient = temp_result;
    remainder = new_remainder;
    result_scale++;
  }

  if (!s21_is_zero_for_div(remainder) && result_scale > 0) {
    s21_decimal test_remainder = remainder;
    if (s21_multiply_by_int(&test_remainder, 10) == S21_DECIMAL_OK) {
      s21_decimal next_digit, final_remainder;
      s21_integer_division(test_remainder, abs_divisor, &next_digit,
                           &final_remainder);

      int should_round_up = 0;
      if (next_digit.bits[0] > 5) {
        should_round_up = 1;
      } else if (next_digit.bits[0] == 5) {
        if (!s21_is_zero_for_div(final_remainder) || (quotient.bits[0] & 1)) {
          should_round_up = 1;
        }
      }

      if (should_round_up) {
        s21_decimal one;
        s21_clean_decimal(&one);
        one.bits[0] = 1;
        s21_decimal temp_result;
        if (s21_add(quotient, one, &temp_result) == S21_DECIMAL_OK) {
          quotient = temp_result;
        }
      }
    }
  }

  while (result_scale > 0 && s21_is_divisible_by_10(quotient)) {
    s21_divide_by_int(&quotient, 10);
    result_scale--;
  }

  while (s21_is_decimal_overflow(&quotient) && result_scale > 0) {
    s21_round_divide_decimal_by_10(&quotient);
    result_scale--;
  }

  while (result_scale > 28) {
    s21_round_divide_decimal_by_10(&quotient);
    result_scale--;
  }

  if (s21_is_decimal_overflow(&quotient)) {
    return result_sign ? S21_DECIMAL_SMALL : S21_DECIMAL_ERROR;
  }

  while (result_scale < 0) {
    if (s21_multiply_by_int(&quotient, 10) != S21_DECIMAL_OK) {
      return result_sign ? S21_DECIMAL_SMALL : S21_DECIMAL_ERROR;
    }
    result_scale++;
  }

  *result = quotient;
  s21_set_decimal_scale(result, result_scale);
  s21_set_decimal_sign(result, result_sign);

  return S21_DECIMAL_OK;
}