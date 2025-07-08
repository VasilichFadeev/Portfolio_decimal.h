#ifndef S21_DECIMAL_H
#define S21_DECIMAL_H

#include <math.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
  uint32_t bits[4];  // [0-2] - мантисса (96 бит), [3] - знак + масштаб
} s21_decimal;

// Арифметические операции
int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

// Функции сравнения
int s21_is_less(s21_decimal value_1, s21_decimal value_2);
int s21_is_less_or_equal(s21_decimal value_1, s21_decimal value_2);
int s21_is_greater(s21_decimal value_1, s21_decimal value_2);
int s21_is_greater_or_equal(s21_decimal value_1, s21_decimal value_2);
int s21_is_equal(s21_decimal value_1, s21_decimal value_2);
int s21_is_not_equal(s21_decimal value_1, s21_decimal value_2);

// Преобразователи
int s21_from_int_to_decimal(int src, s21_decimal *dst);
int s21_from_float_to_decimal(float src, s21_decimal *dst);
int s21_from_decimal_to_int(s21_decimal src, int *dst);
int s21_from_decimal_to_float(s21_decimal src, float *dst);

// Вспомогательные функции
int s21_get_sign(s21_decimal value);
void s21_set_sign(s21_decimal *d, int sign);
int s21_get_scale(s21_decimal *d);
void s21_set_scale(s21_decimal *d, int scale);
void s21_normalize(s21_decimal *value_1, s21_decimal *value_2);
int s21_mul_by_10(s21_decimal *value);
void s21_init_decimal(s21_decimal *d);
int s21_truncate(s21_decimal value, s21_decimal *result);
int s21_get_exponent(s21_decimal value);
void s21_clean_decimal(s21_decimal *dec);
void s21_set_decimal_scale(s21_decimal *value, int scale);
void s21_set_decimal_sign(s21_decimal *value, int sign);
int s21_mod(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
unsigned int s21_get_scale_for_round(s21_decimal value);
int s21_get_second_byte(int value);
void s21_abs_decimal(s21_decimal value, s21_decimal *result);
int s21_multiply_by_int(s21_decimal *value, int multiplier);
int s21_get_decimal_sign(s21_decimal value);
void s21_normalize_decimals(s21_decimal *value_1, s21_decimal *value_2);
int s21_compare_abs_3words(s21_decimal val1, s21_decimal val2);
int s21_is_decimal_correct(s21_decimal value);
int s21_get_decimal_bit(s21_decimal value, int index);
unsigned int s21_divide_by_int(s21_decimal *dec, int divisor);
int s21_is_zero_for_div(s21_decimal value);
int s21_shift_left_for_div(s21_decimal *value, int shift);
void s21_set_decimal_bit(s21_decimal *decimal, int index, int value);
void s21_integer_division(s21_decimal dividend, s21_decimal divisor,
                          s21_decimal *quotient, s21_decimal *remainder);
int s21_is_divisible_by_10(const s21_decimal dec);

void s21_copy_decimal(const s21_decimal *src, s21_decimal *dst);
int s21_floor(s21_decimal value, s21_decimal *result);
int s21_is_zero(const s21_decimal *dec);
int s21_negate(s21_decimal value, s21_decimal *result);
int s21_round(s21_decimal value, s21_decimal *result);

#endif