#include <check.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>

#include "s21_decimal.h"

#define S21_DECIMAL_OK 0
#define S21_DECIMAL_ERROR 1
#define S21_DECIMAL_SMALL 2
#define S21_DECIMAL_ZERO_DIV 3

#define EXIT_SUCCESS 1
#define EXIT_FAILURE 0

START_TEST(test_add) {
  s21_decimal a = {{1, 0, 0, 0}};
  s21_decimal b = {{2, 0, 0, 0}};
  s21_decimal result = {{0, 0, 0, 0}};

  ck_assert_int_eq(s21_add(a, b, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(result.bits[0], 3);
}
END_TEST

START_TEST(
    test_add_79228162514264337593543950335_plus_79228162514264337593543950335) {
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000}};
  s21_decimal b = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000}};
  s21_decimal result = {{0, 0, 0, 0}};

  ck_assert_int_eq(s21_add(a, b, &result), S21_DECIMAL_ERROR);
}
END_TEST

START_TEST(
    test_add_minus_79228162514264337593543950335_plus_79228162514264337593543950335) {
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x80000000}};
  s21_decimal b = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000}};
  s21_decimal result = {{0, 0, 0, 0}};

  ck_assert_int_eq(s21_add(a, b, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(result.bits[0], 0);
}
END_TEST

START_TEST(
    test_add_minus_79228162514264337593543950335_plus_minus_79228162514264337593543950335) {
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x80000000}};
  s21_decimal b = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x80000000}};
  s21_decimal result = {{0, 0, 0, 0}};

  ck_assert_int_eq(s21_add(a, b, &result), S21_DECIMAL_SMALL);
}
END_TEST

START_TEST(test_add_1o234_plus_0o56) {
  s21_decimal a = {{1234, 0, 0, 0x00030000}};  // 1.234
  s21_decimal b = {{56, 0, 0, 0x00020000}};    // 0.56
  s21_decimal result = {0};
  s21_decimal expected = {{1794, 0, 0, 0x00030000}};  // 1.794
  ck_assert_int_eq(s21_add(a, b, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(test_add_overflow) {
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  s21_decimal b = {{1, 0, 0, 0}};
  s21_decimal result = {0};

  ck_assert_int_eq(s21_add(a, b, &result), S21_DECIMAL_ERROR);
}
END_TEST

START_TEST(test_add_simple) {
  s21_decimal a = {{5, 0, 0, 0}};
  s21_decimal b = {{3, 0, 0, 0}};
  s21_decimal result = {0};

  ck_assert_int_eq(s21_add(a, b, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(result.bits[0], 8);
  ck_assert_int_eq(s21_get_sign(result), 0);
}
END_TEST

START_TEST(test_add_negative) {
  s21_decimal a = {{5, 0, 0, 0x80000000}};
  s21_decimal b = {{3, 0, 0, 0x80000000}};
  s21_decimal result = {0};

  ck_assert_int_eq(s21_add(a, b, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(result.bits[0], 8);
  ck_assert_int_eq(s21_get_sign(result), 1);
}
END_TEST

START_TEST(test_add_mixed_signs) {
  s21_decimal a = {{5, 0, 0, 0}};
  s21_decimal b = {{3, 0, 0, 0x80000000}};
  s21_decimal result = {0};

  ck_assert_int_eq(s21_add(a, b, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(result.bits[0], 2);
  ck_assert_int_eq(s21_get_sign(result), 0);
}
END_TEST

START_TEST(test_add_large_numbers) {
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0, 0}};
  s21_decimal b = {{1, 0, 0, 0}};
  s21_decimal result = {0};

  ck_assert_int_eq(s21_add(a, b, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(result.bits[0], 0);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 1);
}
END_TEST

START_TEST(test_add_with_scale) {
  s21_decimal a = {{50, 0, 0, 0x00010000}};  // 5.0
  s21_decimal b = {{30, 0, 0, 0x00010000}};  // 3.0
  s21_decimal result = {0};

  ck_assert_int_eq(s21_add(a, b, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(result.bits[0], 80);
  ck_assert_int_eq(s21_get_exponent(result), 1);
}
END_TEST

START_TEST(test_add_overflow_with_scale) {
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00010000}};
  s21_decimal b = {{1, 0, 0, 0x00010000}};
  s21_decimal result = {0};

  ck_assert_int_eq(s21_add(a, b, &result), S21_DECIMAL_OK);
  // Проверяем, что произошло деление на 10 и уменьшение экспоненты
  ck_assert_int_eq(s21_get_exponent(result), 0);
}
END_TEST

START_TEST(test_add_underflow) {
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x80000000}};
  s21_decimal b = {{1, 0, 0, 0x80000000}};
  s21_decimal result = {0};

  ck_assert_int_eq(s21_add(a, b, &result), S21_DECIMAL_SMALL);
}
END_TEST

START_TEST(test_add_max_scale_reduction) {
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x001C0000}};
  s21_decimal b = {{1, 0, 0, 0x001C0000}};
  s21_decimal result = {0};

  ck_assert_int_eq(s21_add(a, b, &result), S21_DECIMAL_OK);
  // Проверяем, что масштаб уменьшился, а значение корректно
  ck_assert_int_lt(s21_get_exponent(result), 28);
}
END_TEST

START_TEST(test_add_diff_scales) {
  s21_decimal a = {{50, 0, 0, 0x00010000}};  // 5.0
  s21_decimal b = {{3, 0, 0, 0}};            // 3
  s21_decimal result = {0};

  ck_assert_int_eq(s21_add(a, b, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(result.bits[0], 80);
  ck_assert_int_eq(s21_get_exponent(result), 1);
}
END_TEST

START_TEST(test_add_simple_positive) {
  s21_decimal a = {{5, 0, 0, 0}};
  s21_decimal b = {{3, 0, 0, 0}};
  s21_decimal result = {0};
  s21_decimal expected = {{8, 0, 0, 0}};

  ck_assert_int_eq(s21_add(a, b, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(test_add_simple_negative) {
  s21_decimal a = {{5, 0, 0, 0x80000000}};
  s21_decimal b = {{3, 0, 0, 0x80000000}};
  s21_decimal result = {0};
  s21_decimal expected = {{8, 0, 0, 0x80000000}};

  ck_assert_int_eq(s21_add(a, b, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(test_add_mixed_signs_positive_result) {
  s21_decimal a = {{5, 0, 0, 0}};
  s21_decimal b = {{3, 0, 0, 0x80000000}};
  s21_decimal result = {0};
  s21_decimal expected = {{2, 0, 0, 0}};

  ck_assert_int_eq(s21_add(a, b, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(test_add_mixed_signs_negative_result) {
  s21_decimal a = {{3, 0, 0, 0}};
  s21_decimal b = {{5, 0, 0, 0x80000000}};
  s21_decimal result = {0};
  s21_decimal expected = {{2, 0, 0, 0x80000000}};

  ck_assert_int_eq(s21_add(a, b, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(test_add_overflow_max_scale_reduction) {
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x001C0000}};
  s21_decimal b = {{1, 0, 0, 0x001C0000}};
  s21_decimal result = {0};

  ck_assert_int_eq(s21_add(a, b, &result), S21_DECIMAL_OK);
  ck_assert_int_lt(s21_get_exponent(result), 28);
}
END_TEST

START_TEST(test_add_underflow_negative) {
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x80000000}};
  s21_decimal b = {{1, 0, 0, 0x80000000}};
  s21_decimal result = {0};

  ck_assert_int_eq(s21_add(a, b, &result), S21_DECIMAL_SMALL);
}
END_TEST

START_TEST(test_add_zero) {
  s21_decimal a = {{0, 0, 0, 0}};
  s21_decimal b = {{0, 0, 0, 0}};
  s21_decimal result = {0};
  s21_decimal expected = {{0, 0, 0, 0}};

  ck_assert_int_eq(s21_add(a, b, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(test_sub) {
  s21_decimal a = {{5, 0, 0, 0}};
  s21_decimal b = {{3, 0, 0, 0}};
  s21_decimal result = {{0, 0, 0, 0}};

  ck_assert_int_eq(s21_sub(a, b, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(result.bits[0], 2);
}
END_TEST

START_TEST(test_sub_basic) {
  s21_decimal a = {{10, 0, 0, 0}};
  s21_decimal b = {{3, 0, 0, 0}};
  s21_decimal result = {0};
  ck_assert_int_eq(s21_sub(a, b, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(result.bits[0], 7);
}
END_TEST

START_TEST(test_sub_negative_result) {
  s21_decimal a = {{3, 0, 0, 0}};
  s21_decimal b = {{5, 0, 0, 0}};
  s21_decimal result = {0};
  ck_assert_int_eq(s21_sub(a, b, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(result.bits[0], 2);
  ck_assert_int_eq(s21_get_sign(result), 1);
}
END_TEST

START_TEST(test_sub_large_values) {
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  s21_decimal b = {{1, 0, 0, 0}};
  s21_decimal result = {0};
  ck_assert_int_eq(s21_sub(a, b, &result), S21_DECIMAL_OK);
  ck_assert_uint_eq(result.bits[0], 0xFFFFFFFE);
  ck_assert_uint_eq(result.bits[1], 0xFFFFFFFF);
}
END_TEST

START_TEST(test_sub_zero_result) {
  s21_decimal a = {{5, 0, 0, 0}};
  s21_decimal b = {{5, 0, 0, 0}};
  s21_decimal result = {0};
  ck_assert_int_eq(s21_sub(a, b, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(s21_is_zero(&result), 1);
}
END_TEST

START_TEST(test_sub_underflow_negative) {
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x80000000}};
  s21_decimal b = {{1, 0, 0, 0}};
  s21_decimal result = {0};
  ck_assert_int_eq(s21_sub(a, b, &result), S21_DECIMAL_SMALL);
}
END_TEST

START_TEST(test_sub_diff_scales) {
  s21_decimal a = {{50, 0, 0, 0x00010000}};  // 5.0
  s21_decimal b = {{30, 0, 0, 0x00020000}};  // 0.30
  s21_decimal result = {0};
  s21_decimal expected = {{470, 0, 0, 0x00020000}};  // 4.70
  ck_assert_int_eq(s21_sub(a, b, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(test_sub_negative_both) {
  s21_decimal a = {{10, 0, 0, 0x80000000}};  // -10
  s21_decimal b = {{3, 0, 0, 0x80000000}};   // -3
  s21_decimal result = {0};
  s21_decimal expected = {{7, 0, 0, 0x80000000}};  // -7
  ck_assert_int_eq(s21_sub(a, b, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(test_sub_mixed_signs) {
  s21_decimal a = {{10, 0, 0, 0}};          // 10
  s21_decimal b = {{3, 0, 0, 0x80000000}};  // -3
  s21_decimal result = {0};
  s21_decimal expected = {{13, 0, 0, 0}};  // 13
  ck_assert_int_eq(s21_sub(a, b, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(test_sub_max_scale) {
  s21_decimal a = {
      {123456789, 0, 0, 0x001C0000}};       // 0.000000000000000000123456789
  s21_decimal b = {{1, 0, 0, 0x001C0000}};  // 0.000000000000000000000000001
  s21_decimal result = {0};
  s21_decimal expected = {
      {123456788, 0, 0, 0x001C0000}};  // 0.000000000000000000123456788
  ck_assert_int_eq(s21_sub(a, b, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(test_sub_invalid_decimal) {
  s21_decimal a = {
      {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF0000}};  // Неверный масштаб
  s21_decimal b = {{1, 0, 0, 0}};
  s21_decimal result = {0};
  ck_assert_int_eq(s21_sub(a, b, &result), S21_DECIMAL_OK);
}
END_TEST

START_TEST(test_mul) {
  s21_decimal a = {{3, 0, 0, 0}};
  s21_decimal b = {{4, 0, 0, 0}};
  s21_decimal result = {{0, 0, 0, 0}};

  ck_assert_int_eq(s21_mul(a, b, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(result.bits[0], 12);
}
END_TEST

START_TEST(test_mul_basic) {
  s21_decimal a = {{3, 0, 0, 0}};
  s21_decimal b = {{4, 0, 0, 0}};
  s21_decimal result;
  ck_assert_int_eq(s21_mul(a, b, &result), S21_DECIMAL_OK);
  ck_assert_uint_eq(result.bits[0], 12);
  ck_assert_uint_eq(s21_get_exponent(result), 0);
  ck_assert_uint_eq(s21_get_sign(result), 0);
}
END_TEST

START_TEST(test_mul_negative) {
  s21_decimal a = {{5, 0, 0, 0x80000000}};  // -5
  s21_decimal b = {{3, 0, 0, 0}};           // +3
  s21_decimal result;
  ck_assert_int_eq(s21_mul(a, b, &result), S21_DECIMAL_OK);
  ck_assert_uint_eq(result.bits[0], 15);
  ck_assert_uint_eq(s21_get_sign(result), 1);  // Ожидается -15
}
END_TEST

START_TEST(test_mul_overflow) {
  s21_decimal a = {
      {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};  // Максимальное 96-битное число
  s21_decimal b = {{2, 0, 0, 0}};
  s21_decimal result;
  ck_assert_int_eq(s21_mul(a, b, &result), 1);  // Ожидается ошибка переполнения
}
END_TEST

START_TEST(test_mul_zero) {
  s21_decimal a = {{0, 0, 0, 0}};
  s21_decimal b = {{123, 0, 0, 0}};
  s21_decimal result;
  ck_assert_int_eq(s21_mul(a, b, &result), S21_DECIMAL_OK);
  ck_assert_uint_eq(result.bits[0], 0);
}
END_TEST

START_TEST(test_mul_exponent) {
  s21_decimal a = {{123, 0, 0, 0x00020000}};  // 1.23 (exp=2)
  s21_decimal b = {{456, 0, 0, 0x00010000}};  // 4.56 (exp=1)
  s21_decimal result;
  ck_assert_int_eq(s21_mul(a, b, &result), S21_DECIMAL_OK);
  ck_assert_uint_eq(result.bits[0], 123 * 456);    // 56088
  ck_assert_uint_eq(s21_get_exponent(result), 3);  // exp=2+1=3 → 56.088
}
END_TEST

START_TEST(test_mul_fractional) {
  s21_decimal a = {{123456, 0, 0, 0x00030000}};  // 123.456
  s21_decimal b = {{789, 0, 0, 0x00020000}};     // 7.89
  s21_decimal result;
  ck_assert_int_eq(s21_mul(a, b, &result), S21_DECIMAL_OK);
  ck_assert_uint_eq(result.bits[0], 97406784);     // 123456 * 789
  ck_assert_uint_eq(s21_get_exponent(result), 5);  // 3+2=5 → 974.06784
}
END_TEST

START_TEST(test_mul_large_numbers) {
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0, 0}};  // 2^64-1
  s21_decimal b = {{1, 0, 0, 0}};
  s21_decimal result;
  ck_assert_int_eq(s21_mul(a, b, &result), S21_DECIMAL_OK);
  ck_assert_uint_eq(result.bits[0], 0xFFFFFFFF);
  ck_assert_uint_eq(result.bits[1], 0xFFFFFFFF);
}
END_TEST

START_TEST(test_mul_max_scale) {
  s21_decimal a = {{1, 0, 0, 0x001C0000}};  // 1e-28
  s21_decimal b = {{10, 0, 0, 0}};          // 10
  s21_decimal result;
  ck_assert_int_eq(s21_mul(a, b, &result), S21_DECIMAL_OK);
  ck_assert_uint_eq(result.bits[0], 10);            // 1e-28 * 10 = 1e-27
  ck_assert_uint_eq(s21_get_exponent(result), 28);  // Должно остаться 28
}
END_TEST

START_TEST(test_mul_underflow) {
  s21_decimal a = {{1, 0, 0, 0x001C0000}};  // 1e-28
  s21_decimal b = {{1, 0, 0, 0x001C0000}};  // 1e-28
  s21_decimal result;
  ck_assert_int_eq(s21_mul(a, b, &result), S21_DECIMAL_OK);  // 1e-56 → 0
  ck_assert_uint_eq(result.bits[0], 0);
  ck_assert_uint_eq(s21_get_exponent(result), 28);
}
END_TEST

START_TEST(test_mul_infinity) {
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  s21_decimal b = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  s21_decimal result;
  ck_assert_int_eq(s21_mul(a, b, &result), 1);  // Ожидается ошибка overflow
}
END_TEST

START_TEST(test_mul_both_negative) {
  s21_decimal a = {{5, 0, 0, 0x80000000}};  // -5
  s21_decimal b = {{3, 0, 0, 0x80000000}};  // -3
  s21_decimal result;
  ck_assert_int_eq(s21_mul(a, b, &result), S21_DECIMAL_OK);
  ck_assert_uint_eq(result.bits[0], 15);
  ck_assert_uint_eq(s21_get_sign(result), 0);  // Ожидается +15
}
END_TEST

START_TEST(test_mul_invalid_decimal) {
  s21_decimal a = {{3, 0, 0, 0xFFFF0000}};  // Неверный масштаб
  s21_decimal b = {{4, 0, 0, 0}};
  s21_decimal result;
  ck_assert_int_eq(s21_mul(a, b, &result), S21_DECIMAL_ERROR);
}
END_TEST

START_TEST(test_mul_large_scale_reduction) {
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
                    0x001C0000}};  // Максимальное с scale=28
  s21_decimal b = {{2, 0, 0, 0}};
  s21_decimal result;
  ck_assert_int_eq(s21_mul(a, b, &result), S21_DECIMAL_OK);
  ck_assert_int_lt(s21_get_exponent(result), 28);  // Масштаб должен уменьшиться
}
END_TEST

START_TEST(test_div) {
  s21_decimal a = {{10, 0, 0, 0}};
  s21_decimal b = {{2, 0, 0, 0}};
  s21_decimal result = {{0, 0, 0, 0}};

  ck_assert_int_eq(s21_div(a, b, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(result.bits[0], 5);
}
END_TEST

START_TEST(test_div_by_zero) {
  s21_decimal a = {{10, 0, 0, 0}};
  s21_decimal b = {{0, 0, 0, 0}};
  s21_decimal result = {0};

  ck_assert_int_eq(s21_div(a, b, &result), S21_DECIMAL_ZERO_DIV);
}
END_TEST

START_TEST(test_div_basic) {
  s21_decimal a = {{10, 0, 0, 0}};
  s21_decimal b = {{2, 0, 0, 0}};
  s21_decimal result;
  ck_assert_int_eq(s21_div(a, b, &result), S21_DECIMAL_OK);
  ck_assert_uint_eq(result.bits[0], 5);
}
END_TEST

START_TEST(test_div_negative) {
  s21_decimal a = {{15, 0, 0, 0x80000000}};  // -15
  s21_decimal b = {{3, 0, 0, 0}};
  s21_decimal result;
  ck_assert_int_eq(s21_div(a, b, &result), S21_DECIMAL_OK);
  ck_assert_uint_eq(result.bits[0], 5);
  ck_assert_uint_eq(s21_get_sign(result), 1);  // -5
}
END_TEST

START_TEST(test_div_large_numbers) {
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};  // MAX
  s21_decimal b = {{1, 0, 0, 0}};
  s21_decimal result;
  ck_assert_int_eq(s21_div(a, b, &result), S21_DECIMAL_OK);
  ck_assert_uint_eq(result.bits[0], 0xFFFFFFFF);
  ck_assert_uint_eq(result.bits[1], 0xFFFFFFFF);
  ck_assert_uint_eq(result.bits[2], 0xFFFFFFFF);
}
END_TEST

START_TEST(test_div_fractional) {
  s21_decimal a = {{1, 0, 0, 0}};  // 1
  s21_decimal b = {{2, 0, 0, 0}};  // 2
  s21_decimal result;
  s21_decimal expected = {{5, 0, 0, 0x00010000}};  // 0.5
  ck_assert_int_eq(s21_div(a, b, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(test_div_large_scale) {
  s21_decimal a = {{1, 0, 0, 0x001C0000}};  // 1e-28
  s21_decimal b = {{2, 0, 0, 0}};           // 2
  s21_decimal result;
  ck_assert_int_eq(s21_div(a, b, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(s21_is_zero(&result), 1);
}
END_TEST

START_TEST(test_div_both_negative) {
  s21_decimal a = {{15, 0, 0, 0x80000000}};  // -15
  s21_decimal b = {{3, 0, 0, 0x80000000}};   // -3
  s21_decimal result;
  ck_assert_int_eq(s21_div(a, b, &result), S21_DECIMAL_OK);
  ck_assert_uint_eq(result.bits[0], 5);
  ck_assert_uint_eq(s21_get_sign(result), 0);  // +5
}
END_TEST

START_TEST(test_div_null_result) {
  s21_decimal a = {{10, 0, 0, 0}};
  s21_decimal b = {{2, 0, 0, 0}};
  ck_assert_int_eq(s21_div(a, b, NULL), S21_DECIMAL_ERROR);
}
END_TEST

START_TEST(test_div_invalid_decimal) {
  s21_decimal a = {{10, 0, 0, 0xFFFF0000}};  // Неверный масштаб
  s21_decimal b = {{2, 0, 0, 0}};
  s21_decimal result;
  ck_assert_int_eq(s21_div(a, b, &result), S21_DECIMAL_ERROR);
}
END_TEST

START_TEST(test_mod_normal) {
  s21_decimal a = {{10, 0, 0, 0}};
  s21_decimal b = {{3, 0, 0, 0}};
  s21_decimal result = {0};
  s21_decimal expected = {{1, 0, 0, 0}};

  ck_assert_int_eq(s21_mod(a, b, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(test_mod_negative) {
  s21_decimal a = {{10, 0, 0, 0x80000000}};
  s21_decimal b = {{3, 0, 0, 0}};
  s21_decimal result = {0};
  s21_decimal expected = {{1, 0, 0, 0x80000000}};

  ck_assert_int_eq(s21_mod(a, b, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(test_mod_zero_divisor) {
  s21_decimal a = {{10, 0, 0, 0}};
  s21_decimal b = {{0, 0, 0, 0}};
  s21_decimal result = {0};

  ck_assert_int_eq(s21_mod(a, b, &result), S21_DECIMAL_ZERO_DIV);
}
END_TEST

START_TEST(test_get_sign_positive) {
  s21_decimal dec = {{0, 0, 0, 0}};
  ck_assert_int_eq(s21_get_sign(dec), 0);
}
END_TEST

START_TEST(test_get_sign_negative) {
  s21_decimal dec = {{0, 0, 0, 0x80000000}};
  ck_assert_int_eq(s21_get_sign(dec), 1);
}
END_TEST

START_TEST(test_is_equal_true) {
  s21_decimal a = {{123, 456, 789, 0}};
  s21_decimal b = {{123, 456, 789, 0}};
  ck_assert_int_eq(s21_is_equal(a, b), 1);
}
END_TEST

START_TEST(test_is_equal_false) {
  s21_decimal a = {{123, 0, 0, 0}};
  s21_decimal b = {{456, 0, 0, 0}};
  ck_assert_int_eq(s21_is_equal(a, b), 0);
}
END_TEST

START_TEST(test_is_equal_normalized) {
  s21_decimal a = {{123, 0, 0, 0x00020000}};
  s21_decimal b = {{1230, 0, 0, 0x00030000}};
  ck_assert_int_eq(s21_is_equal(a, b), 1);
}
END_TEST

START_TEST(test_is_less_positive) {
  s21_decimal a = {{5, 0, 0, 0}};
  s21_decimal b = {{10, 0, 0, 0}};
  ck_assert_int_eq(s21_is_less(a, b), 1);
  ck_assert_int_eq(s21_is_less(b, a), 0);
}
END_TEST

START_TEST(test_is_less_negative) {
  s21_decimal a = {{5, 0, 0, 0x80000000}};
  s21_decimal b = {{10, 0, 0, 0x80000000}};
  ck_assert_int_eq(s21_is_less(a, b), 0);
  ck_assert_int_eq(s21_is_less(b, a), 1);
}
END_TEST

START_TEST(test_is_less_diff_sign) {
  s21_decimal a = {{5, 0, 0, 0x80000000}};
  s21_decimal b = {{10, 0, 0, 0}};
  ck_assert_int_eq(s21_is_less(a, b), 1);
  ck_assert_int_eq(s21_is_less(b, a), 0);
}
END_TEST

START_TEST(test_is_greater_true) {
  s21_decimal a = {{10, 0, 0, 0}};
  s21_decimal b = {{5, 0, 0, 0}};
  ck_assert_int_eq(s21_is_greater(a, b), 1);
}
END_TEST

START_TEST(test_is_greater_false) {
  s21_decimal a = {{5, 0, 0, 0}};
  s21_decimal b = {{10, 0, 0, 0}};
  ck_assert_int_eq(s21_is_greater(a, b), 0);
}
END_TEST

START_TEST(test_is_greater_or_equal_equal) {
  s21_decimal a = {{5, 0, 0, 0}};
  s21_decimal b = {{5, 0, 0, 0}};
  ck_assert_int_eq(s21_is_greater_or_equal(a, b), 1);
}
END_TEST

START_TEST(test_is_greater_or_equal_greater) {
  s21_decimal a = {{10, 0, 0, 0}};
  s21_decimal b = {{5, 0, 0, 0}};
  ck_assert_int_eq(s21_is_greater_or_equal(a, b), 1);
}
END_TEST

START_TEST(test_is_greater_or_equal_less) {
  s21_decimal a = {{5, 0, 0, 0}};
  s21_decimal b = {{10, 0, 0, 0}};
  ck_assert_int_eq(s21_is_greater_or_equal(a, b), 0);
}
END_TEST

START_TEST(test_is_less_or_equal_less) {
  s21_decimal a = {{5, 0, 0, 0}};
  s21_decimal b = {{10, 0, 0, 0}};
  ck_assert_int_eq(s21_is_less_or_equal(a, b), 1);
}
END_TEST

START_TEST(test_is_less_or_equal_equal) {
  s21_decimal a = {{5, 0, 0, 0}};
  s21_decimal b = {{5, 0, 0, 0}};
  ck_assert_int_eq(s21_is_less_or_equal(a, b), 1);
}
END_TEST

START_TEST(test_is_less_or_equal_greater) {
  s21_decimal a = {{10, 0, 0, 0}};
  s21_decimal b = {{5, 0, 0, 0}};
  ck_assert_int_eq(s21_is_less_or_equal(a, b), 0);
}
END_TEST

START_TEST(test_is_not_equal_true) {
  s21_decimal a = {{1, 0, 0, 0}};
  s21_decimal b = {{2, 0, 0, 0}};
  ck_assert_int_eq(s21_is_not_equal(a, b), 1);
}
END_TEST

START_TEST(test_is_not_equal_false) {
  s21_decimal a = {{2, 0, 0, 0}};
  s21_decimal b = {{2, 0, 0, 0}};
  ck_assert_int_eq(s21_is_not_equal(a, b), 0);
}
END_TEST

START_TEST(test_mul_by_10) {
  s21_decimal value = {{2, 0, 0, 0}};
  s21_mul_by_10(&value);
  ck_assert_int_eq(value.bits[0], 20);
}
END_TEST

START_TEST(test_mul_by_10_large) {
  s21_decimal value = {{12345, 0, 0, 0}};
  s21_mul_by_10(&value);
  ck_assert_int_eq(value.bits[0], 123450);
}
END_TEST

START_TEST(test_from_int_to_dec_basic) {
  s21_decimal dec;
  int src = 12345;
  ck_assert_int_eq(s21_from_int_to_decimal(src, &dec), S21_DECIMAL_OK);
  ck_assert_int_eq(dec.bits[0], 12345);
  ck_assert_int_eq(s21_get_sign(dec), 0);
}
END_TEST

START_TEST(test_from_int_to_dec_negative) {
  s21_decimal dec;
  int src = -6789;
  ck_assert_int_eq(s21_from_int_to_decimal(src, &dec), S21_DECIMAL_OK);
  ck_assert_int_eq(dec.bits[0], 6789);
  ck_assert_int_eq(s21_get_sign(dec), 1);
}
END_TEST

START_TEST(test_from_int_to_dec_max) {
  s21_decimal dec;
  ck_assert_int_eq(s21_from_int_to_decimal(INT_MAX, &dec), S21_DECIMAL_OK);
  ck_assert_int_eq(dec.bits[0], INT_MAX);
}
END_TEST

START_TEST(test_from_float_to_dec_normal) {
  s21_decimal dec;
  float src = 123.4567f;
  ck_assert_int_eq(s21_from_float_to_decimal(src, &dec), S21_DECIMAL_OK);

  float res;
  s21_from_decimal_to_float(dec, &res);
  ck_assert_float_eq_tol(res, src, 1e-6);
}
END_TEST

START_TEST(test_from_float_to_dec_small) {
  s21_decimal dec;
  float src = 1e-29f;
  ck_assert_int_eq(s21_from_float_to_decimal(src, &dec), S21_DECIMAL_ERROR);
  ck_assert_int_eq(dec.bits[0], 0);
}
END_TEST

START_TEST(test_from_float_to_dec_negative) {
  s21_decimal dec;
  float src = -123.4567f;
  ck_assert_int_eq(s21_from_float_to_decimal(src, &dec), S21_DECIMAL_OK);
  ck_assert_uint_eq(s21_get_sign(dec), 1);
  float res;
  s21_from_decimal_to_float(dec, &res);
  ck_assert_float_eq_tol(res, src, 1e-6);
}
END_TEST

START_TEST(test_from_float_to_dec_max_precision) {
  s21_decimal dec;
  float src = 0.0000000000000000000000000001f;  // 1e-28
  ck_assert_int_eq(s21_from_float_to_decimal(src, &dec), S21_DECIMAL_OK);
  ck_assert_uint_eq(dec.bits[0], 1);
  ck_assert_uint_eq(s21_get_exponent(dec), 28);
}
END_TEST

START_TEST(test_from_float_to_dec_null_dst) {
  float src = 123.456f;
  ck_assert_int_eq(s21_from_float_to_decimal(src, NULL), S21_DECIMAL_ERROR);
}
END_TEST

START_TEST(test_from_dec_to_int_truncate) {
  s21_decimal dec;
  s21_from_float_to_decimal(123.999f, &dec);

  int res;
  ck_assert_int_eq(s21_from_decimal_to_int(dec, &res), S21_DECIMAL_OK);
  ck_assert_int_eq(res, 124);
}
END_TEST

START_TEST(test_from_dec_to_int_overflow) {
  s21_decimal dec;
  s21_from_float_to_decimal(2147483648.0f, &dec);

  int res;
  ck_assert_int_eq(s21_from_decimal_to_int(dec, &res), S21_DECIMAL_ERROR);
}
END_TEST

START_TEST(test_from_dec_to_int_null_dst) {
  s21_decimal dec = {{123, 0, 0, 0}};
  ck_assert_int_eq(s21_from_decimal_to_int(dec, NULL), S21_DECIMAL_ERROR);
}
END_TEST

START_TEST(test_from_dec_to_int_rounding_negative) {
  s21_decimal dec;
  s21_from_float_to_decimal(-123.6f, &dec);

  int res;
  ck_assert_int_eq(s21_from_decimal_to_int(dec, &res), S21_DECIMAL_OK);
  ck_assert_int_eq(res, -124);
}
END_TEST

START_TEST(test_from_dec_to_float_precision) {
  s21_decimal dec;
  s21_from_float_to_decimal(12345.6789f, &dec);

  float res;
  s21_from_decimal_to_float(dec, &res);
  ck_assert_float_eq_tol(res, 12345.6789f, 1e-02);
}
END_TEST

START_TEST(test_from_dec_to_float_large) {
  s21_decimal dec = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  s21_set_scale(&dec, 28);

  float res;
  s21_from_decimal_to_float(dec, &res);
  ck_assert_float_eq_tol(res, 79228162514264337593543950335e-28f, 1e-6);
}
END_TEST

START_TEST(s21_floor_positive_1) {
  s21_decimal value = {{123456, 0, 0, 0x00030000}};
  s21_decimal result = {0};
  s21_decimal expected = {{123, 0, 0, 0}};

  ck_assert_int_eq(s21_floor(value, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(s21_floor_positive_2) {
  s21_decimal value = {{123999, 0, 0, 0x00030000}};
  s21_decimal result = {0};
  s21_decimal expected = {{123, 0, 0, 0}};

  ck_assert_int_eq(s21_floor(value, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(s21_round_positive_3) {
  s21_decimal value = {{123654, 0, 0, 0x00030000}};
  s21_decimal result = {0};
  s21_decimal expected = {{124, 0, 0, 0}};

  ck_assert_int_eq(s21_round(value, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(s21_round_exact_half) {
  s21_decimal value = {{5, 0, 0, 0x00010000}};  // 0.5
  s21_decimal result = {0};
  s21_decimal expected = {{1, 0, 0, 0}};  // Ожидаем 1

  ck_assert_int_eq(s21_round(value, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(s21_round_negative) {
  s21_decimal value = {{123654, 0, 0, 0x80030000}};  // -123.654
  s21_decimal result = {0};
  s21_decimal expected = {{124, 0, 0, 0x80000000}};  // Ожидаем -124

  ck_assert_int_eq(s21_round(value, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(s21_round_less_than_half) {
  s21_decimal value = {{123, 0, 0, 0x00030000}};  // 0.123
  s21_decimal result = {0};
  s21_decimal expected = {{0, 0, 0, 0}};  // Ожидаем 0

  ck_assert_int_eq(s21_round(value, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(s21_round_large_scale) {
  s21_decimal value = {{123456789, 0, 0, 0x00060000}};  // 123.456789 (scale 6)
  s21_decimal result = {0};
  s21_decimal expected = {{123, 0, 0, 0}};  // Ожидаем 123

  ck_assert_int_eq(s21_round(value, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(test_round_positive_fraction_gt_half) {
  s21_decimal value = {{123456, 0, 0, 0x00050000}};  // 1.23456 (scale 5)
  s21_decimal result = {0};
  s21_decimal expected = {{1, 0, 0, 0}};  // Ожидаем 1

  ck_assert_int_eq(s21_round(value, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(test_round_positive_fraction_lt_half) {
  s21_decimal value = {{123400, 0, 0, 0x00050000}};  // 1.23400 (scale 5)
  s21_decimal result = {0};
  s21_decimal expected = {{1, 0, 0, 0}};  // Ожидаем 1

  ck_assert_int_eq(s21_round(value, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(test_round_negative_fraction_gt_half) {
  s21_decimal value = {{123456, 0, 0, 0x80050000}};  // -1.23456 (scale 5)
  s21_decimal result = {0};
  s21_decimal expected = {{1, 0, 0, 0x80000000}};  // Ожидаем -1

  ck_assert_int_eq(s21_round(value, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(test_round_negative_fraction_lt_half) {
  s21_decimal value = {{123400, 0, 0, 0x80050000}};  // -1.23400 (scale 5)
  s21_decimal result = {0};
  s21_decimal expected = {{1, 0, 0, 0x80000000}};  // Ожидаем -1

  ck_assert_int_eq(s21_round(value, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(test_round_zero) {
  s21_decimal value = {{0, 0, 0, 0x00030000}};  // 0.000 (scale 3)
  s21_decimal result = {0};
  s21_decimal expected = {{0, 0, 0, 0}};  // Ожидаем 0

  ck_assert_int_eq(s21_round(value, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(test_round_large_number) {
  s21_decimal value = {{123456789, 0, 0, 0x00060000}};  // 123.456789 (scale 6)
  s21_decimal result = {0};
  s21_decimal expected = {{123, 0, 0, 0}};  // Ожидаем 123

  ck_assert_int_eq(s21_round(value, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(test_round_edge_case_just_below_half) {
  s21_decimal value = {{499999, 0, 0, 0x00060000}};  // 0.499999 (scale 6)
  s21_decimal result = {0};
  s21_decimal expected = {{0, 0, 0, 0}};  // Ожидаем 0

  ck_assert_int_eq(s21_round(value, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(test_round_edge_case_just_above_half) {
  s21_decimal value = {{500001, 0, 0, 0x00060000}};  // 0.500001 (scale 6)
  s21_decimal result = {0};
  s21_decimal expected = {{1, 0, 0, 0}};  // Ожидаем 1

  ck_assert_int_eq(s21_round(value, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(test_round_null_result_pointer) {
  s21_decimal value = {{123456, 0, 0, 0x00030000}};

  ck_assert_int_eq(s21_round(value, NULL), S21_DECIMAL_ERROR);
}
END_TEST

START_TEST(s21_truncate_positive_1) {
  s21_decimal value = {{123456, 0, 0, 0x00030000}};
  s21_decimal result = {0};
  s21_decimal expected = {{123, 0, 0, 0}};

  ck_assert_int_eq(s21_truncate(value, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(s21_truncate_negative_1) {
  s21_decimal value = {{123456, 0, 0, 0x80030000}};
  s21_decimal result = {0};
  s21_decimal expected = {{123, 0, 0, 0x80000000}};

  ck_assert_int_eq(s21_truncate(value, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(s21_truncate_negative_2) {
  s21_decimal value = {{123999, 0, 0, 0x80030000}};
  s21_decimal result = {0};
  s21_decimal expected = {{123, 0, 0, 0x80000000}};

  ck_assert_int_eq(s21_truncate(value, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(s21_truncate_large) {
  s21_decimal value = {{123456789, 0, 0, 0x00060000}};
  s21_decimal result = {0};
  s21_decimal expected = {{123, 0, 0, 0}};

  ck_assert_int_eq(s21_truncate(value, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(s21_negate_positive) {
  s21_decimal value = {{12345, 0, 0, 0}};
  s21_decimal result = {0};
  s21_decimal expected = {{12345, 0, 0, 0x80000000}};

  ck_assert_int_eq(s21_negate(value, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(s21_negate_negative) {
  s21_decimal value = {{12345, 0, 0, 0x80000000}};
  s21_decimal result = {0};
  s21_decimal expected = {{12345, 0, 0, 0}};

  ck_assert_int_eq(s21_negate(value, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(s21_negate_zero) {
  s21_decimal value = {{0, 0, 0, 0}};
  s21_decimal result = {0};
  s21_decimal expected = {{0, 0, 0, 0x80000000}};

  ck_assert_int_eq(s21_negate(value, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(s21_negate_max) {
  s21_decimal value = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  s21_decimal result = {0};
  s21_decimal expected = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x80000000}};

  ck_assert_int_eq(s21_negate(value, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(s21_negate_max_scale) {
  s21_decimal value = {{123, 0, 0, 0x001C0000}};
  s21_decimal result = {0};
  s21_decimal expected = {{123, 0, 0, 0x801C0000}};

  ck_assert_int_eq(s21_negate(value, &result), S21_DECIMAL_OK);
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(test_max_decimal) {
  s21_decimal max = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  s21_decimal one = {{1, 0, 0, 0}};
  s21_decimal result = {0};

  ck_assert_int_eq(s21_add(max, one, &result), S21_DECIMAL_ERROR);
}
END_TEST

START_TEST(s21_floor_null_result) {
  s21_decimal value = {{12345, 0, 0, 0}};
  int ret = s21_floor(value, NULL);
  ck_assert_int_eq(ret, 1);
}
END_TEST

START_TEST(s21_floor_negative_integer) {
  s21_decimal value = {{123, 0, 0, 0x80000000}};  // -123
  s21_decimal result = {0};
  s21_decimal expected = {{123, 0, 0, 0x80000000}};  // -123
  int ret = s21_floor(value, &result);
  ck_assert_int_eq(ret, 0);
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(s21_floor_negative_fractional) {
  s21_decimal value = {{12345, 0, 0, 0x80020000}};   // -123.45 (scale 2)
  s21_decimal expected = {{124, 0, 0, 0x80000000}};  // -124
  s21_decimal result = {0};
  int ret = s21_floor(value, &result);
  ck_assert_int_eq(ret, 0);
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(s21_floor_positive_fractional) {
  s21_decimal value = {{12345, 0, 0, 0x00020000}};  // 123.45 (scale 2)
  s21_decimal expected = {{123, 0, 0, 0}};          // 123
  s21_decimal result = {0};
  int ret = s21_floor(value, &result);
  ck_assert_int_eq(ret, 0);
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(s21_floor_zero) {
  s21_decimal value = {{0, 0, 0, 0}};
  s21_decimal expected = {{0, 0, 0, 0}};
  s21_decimal result = {0};
  int ret = s21_floor(value, &result);
  ck_assert_int_eq(ret, 0);
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(s21_floor_positive_integer) {
  s21_decimal value = {{456, 0, 0, 0}};  // 456
  s21_decimal expected = {{456, 0, 0, 0}};
  s21_decimal result = {0};
  int ret = s21_floor(value, &result);
  ck_assert_int_eq(ret, 0);
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(s21_floor_negative_zero_scale) {
  s21_decimal value = {{12300, 0, 0, 0x80020000}};   // -123.00 (scale 2)
  s21_decimal expected = {{123, 0, 0, 0x80000000}};  // -123
  s21_decimal result = {0};
  int ret = s21_floor(value, &result);
  ck_assert_int_eq(ret, 0);
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

START_TEST(s21_floor_max_scale) {
  s21_decimal value = {{1, 0, 0, 0x001C0000}};  // 1e-28
  s21_decimal expected = {{0, 0, 0, 0}};
  s21_decimal result = {0};
  int ret = s21_floor(value, &result);
  ck_assert_int_eq(ret, 0);
  ck_assert_int_eq(s21_is_equal(result, expected), 1);
}
END_TEST

Suite *s21_decimal_suite(void) {
  Suite *s = suite_create("s21_decimal");
  TCase *tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_add);
  tcase_add_test(
      tc_core,
      test_add_79228162514264337593543950335_plus_79228162514264337593543950335);
  tcase_add_test(
      tc_core,
      test_add_minus_79228162514264337593543950335_plus_79228162514264337593543950335);
  tcase_add_test(
      tc_core,
      test_add_minus_79228162514264337593543950335_plus_minus_79228162514264337593543950335);
  tcase_add_test(tc_core, test_add_1o234_plus_0o56);
  tcase_add_test(tc_core, test_add_overflow);
  tcase_add_test(tc_core, test_add_simple);
  tcase_add_test(tc_core, test_add_negative);
  tcase_add_test(tc_core, test_add_mixed_signs);
  tcase_add_test(tc_core, test_add_large_numbers);
  tcase_add_test(tc_core, test_add_with_scale);
  tcase_add_test(tc_core, test_add_overflow_with_scale);
  tcase_add_test(tc_core, test_add_underflow);
  tcase_add_test(tc_core, test_add_max_scale_reduction);
  tcase_add_test(tc_core, test_add_diff_scales);
  tcase_add_test(tc_core, test_add_simple_positive);
  tcase_add_test(tc_core, test_add_simple_negative);
  tcase_add_test(tc_core, test_add_mixed_signs_positive_result);
  tcase_add_test(tc_core, test_add_mixed_signs_negative_result);
  tcase_add_test(tc_core, test_add_overflow_max_scale_reduction);
  tcase_add_test(tc_core, test_add_underflow_negative);
  tcase_add_test(tc_core, test_add_zero);
  tcase_add_test(tc_core, test_sub);
  tcase_add_test(tc_core, test_sub_basic);
  tcase_add_test(tc_core, test_sub_negative_result);
  tcase_add_test(tc_core, test_sub_large_values);
  tcase_add_test(tc_core, test_sub_zero_result);
  tcase_add_test(tc_core, test_sub_underflow_negative);
  tcase_add_test(tc_core, test_sub_diff_scales);
  tcase_add_test(tc_core, test_sub_negative_both);
  tcase_add_test(tc_core, test_sub_mixed_signs);
  tcase_add_test(tc_core, test_sub_max_scale);
  tcase_add_test(tc_core, test_sub_invalid_decimal);
  tcase_add_test(tc_core, test_mul);
  tcase_add_test(tc_core, test_mul_basic);
  tcase_add_test(tc_core, test_mul_negative);
  tcase_add_test(tc_core, test_mul_overflow);
  tcase_add_test(tc_core, test_mul_zero);
  tcase_add_test(tc_core, test_mul_exponent);
  tcase_add_test(tc_core, test_mul_fractional);
  tcase_add_test(tc_core, test_mul_large_numbers);
  tcase_add_test(tc_core, test_mul_max_scale);
  tcase_add_test(tc_core, test_mul_underflow);
  tcase_add_test(tc_core, test_mul_infinity);
  tcase_add_test(tc_core, test_mul_both_negative);
  tcase_add_test(tc_core, test_mul_invalid_decimal);
  tcase_add_test(tc_core, test_mul_large_scale_reduction);
  tcase_add_test(tc_core, test_div);
  tcase_add_test(tc_core, test_div_by_zero);
  tcase_add_test(tc_core, test_div_basic);
  tcase_add_test(tc_core, test_div_negative);
  tcase_add_test(tc_core, test_div_large_numbers);
  tcase_add_test(tc_core, test_div_fractional);
  tcase_add_test(tc_core, test_div_large_scale);
  tcase_add_test(tc_core, test_div_both_negative);
  tcase_add_test(tc_core, test_div_null_result);
  tcase_add_test(tc_core, test_div_invalid_decimal);
  tcase_add_test(tc_core, test_mod_normal);
  tcase_add_test(tc_core, test_mod_negative);
  tcase_add_test(tc_core, test_mod_zero_divisor);
  tcase_add_test(tc_core, test_get_sign_positive);
  tcase_add_test(tc_core, test_get_sign_negative);
  tcase_add_test(tc_core, test_is_equal_true);
  tcase_add_test(tc_core, test_is_equal_false);
  tcase_add_test(tc_core, test_is_equal_normalized);
  tcase_add_test(tc_core, test_is_less_positive);
  tcase_add_test(tc_core, test_is_less_negative);
  tcase_add_test(tc_core, test_is_less_diff_sign);
  tcase_add_test(tc_core, test_is_greater_true);
  tcase_add_test(tc_core, test_is_greater_false);
  tcase_add_test(tc_core, test_is_greater_or_equal_equal);
  tcase_add_test(tc_core, test_is_greater_or_equal_greater);
  tcase_add_test(tc_core, test_is_greater_or_equal_less);
  tcase_add_test(tc_core, test_is_less_or_equal_less);
  tcase_add_test(tc_core, test_is_less_or_equal_equal);
  tcase_add_test(tc_core, test_is_less_or_equal_greater);
  tcase_add_test(tc_core, test_is_not_equal_true);
  tcase_add_test(tc_core, test_is_not_equal_false);
  tcase_add_test(tc_core, test_mul_by_10);
  tcase_add_test(tc_core, test_mul_by_10_large);
  tcase_add_test(tc_core, test_from_int_to_dec_basic);
  tcase_add_test(tc_core, test_from_int_to_dec_negative);
  tcase_add_test(tc_core, test_from_int_to_dec_max);
  tcase_add_test(tc_core, test_from_float_to_dec_normal);
  tcase_add_test(tc_core, test_from_float_to_dec_small);
  tcase_add_test(tc_core, test_from_float_to_dec_negative);
  tcase_add_test(tc_core, test_from_float_to_dec_max_precision);
  tcase_add_test(tc_core, test_from_float_to_dec_null_dst);
  tcase_add_test(tc_core, test_from_dec_to_int_truncate);
  tcase_add_test(tc_core, test_from_dec_to_int_overflow);
  tcase_add_test(tc_core, test_from_dec_to_int_null_dst);
  tcase_add_test(tc_core, test_from_dec_to_int_rounding_negative);
  tcase_add_test(tc_core, test_from_dec_to_float_precision);
  tcase_add_test(tc_core, test_from_dec_to_float_large);
  tcase_add_test(tc_core, s21_floor_positive_1);
  tcase_add_test(tc_core, s21_floor_positive_2);
  tcase_add_test(tc_core, s21_round_positive_3);
  tcase_add_test(tc_core, s21_round_exact_half);
  tcase_add_test(tc_core, s21_round_negative);
  tcase_add_test(tc_core, s21_round_less_than_half);
  tcase_add_test(tc_core, s21_round_large_scale);
  tcase_add_test(tc_core, test_round_positive_fraction_gt_half);
  tcase_add_test(tc_core, test_round_positive_fraction_lt_half);
  tcase_add_test(tc_core, test_round_negative_fraction_gt_half);
  tcase_add_test(tc_core, test_round_negative_fraction_lt_half);
  tcase_add_test(tc_core, test_round_zero);
  tcase_add_test(tc_core, test_round_large_number);
  tcase_add_test(tc_core, test_round_edge_case_just_below_half);
  tcase_add_test(tc_core, test_round_edge_case_just_above_half);
  tcase_add_test(tc_core, test_round_null_result_pointer);
  tcase_add_test(tc_core, s21_truncate_positive_1);
  tcase_add_test(tc_core, s21_truncate_negative_1);
  tcase_add_test(tc_core, s21_truncate_negative_2);
  tcase_add_test(tc_core, s21_truncate_large);
  tcase_add_test(tc_core, s21_negate_positive);
  tcase_add_test(tc_core, s21_negate_negative);
  tcase_add_test(tc_core, s21_negate_zero);
  tcase_add_test(tc_core, s21_negate_max);
  tcase_add_test(tc_core, s21_negate_max_scale);
  tcase_add_test(tc_core, test_max_decimal);
  tcase_add_test(tc_core, s21_floor_null_result);
  tcase_add_test(tc_core, s21_floor_negative_integer);
  tcase_add_test(tc_core, s21_floor_negative_fractional);
  tcase_add_test(tc_core, s21_floor_positive_fractional);
  tcase_add_test(tc_core, s21_floor_zero);
  tcase_add_test(tc_core, s21_floor_positive_integer);
  tcase_add_test(tc_core, s21_floor_negative_zero_scale);
  tcase_add_test(tc_core, s21_floor_max_scale);
  suite_add_tcase(s, tc_core);
  return s;
}

int main(void) {
  Suite *s = s21_decimal_suite();
  SRunner *sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  srunner_free(sr);

  return 0;
}