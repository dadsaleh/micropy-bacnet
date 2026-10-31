#include "cursor.h"
#include "unity.h"
#include <stdint.h>
#include <string.h>

// 33, 244, 35, 52
static const uint8_t fixture[] = {0x21, 0xF4, 0x23, 0x34};
static Cursor c;

void tearDown(void) {}
void setUp(void) { c = make_cursor(fixture, sizeof(fixture)); }

void test_reads_one_byte(void) {
  uint32_t val = 0xDEADBEEF;
  TEST_ASSERT_EQUAL_INT(OK, c_select(&val, &c, 1, 0));
  TEST_ASSERT_EQUAL_HEX32(0x21, val);
}

void test_reads_two_byte(void) {
  uint32_t val = 0xDEADBEEF;
  TEST_ASSERT_EQUAL_INT(OK, c_select(&val, &c, 2, 0));
  TEST_ASSERT_EQUAL_HEX32(0x21F4, val);
}

void test_reads_three_byte(void) {
  uint32_t val = 0xDEADBEEF;
  TEST_ASSERT_EQUAL_INT(OK, c_select(&val, &c, 3, 0));
  TEST_ASSERT_EQUAL_HEX32(0x21F423, val);
}

void test_reads_four_byte(void) {
  uint32_t val = 0xDEADBEEF;
  TEST_ASSERT_EQUAL_INT(OK, c_select(&val, &c, 4, 0));
  TEST_ASSERT_EQUAL_HEX32(0x21F42334, val);
}

void test_reads_high_bit_bytes(void) {
  const uint8_t high[] = {0xFF, 0xFF, 0xFF, 0xFF};
  uint32_t val = 0;
  for (size_t n = 1; n <= 4; n++) {
    c = make_cursor(high, sizeof(high));
    TEST_ASSERT_EQUAL_INT(OK, c_select(&val, &c, n, 0));
    TEST_ASSERT_EQUAL_HEX32(0xFFFFFFFFu >> (32 - n * 8), val);
  }
}

void test_reads_exact_fit_at_end(void) {
  const uint32_t expected[] = {0x34, 0x2334, 0xF42334, 0x21F42334};
  for (size_t n = 1; n <= 4; n++) {
    uint32_t val = 0xDEADBEEF;
    c.pos = sizeof(fixture) - n;
    TEST_ASSERT_EQUAL_INT(OK, c_select(&val, &c, n, n));
    TEST_ASSERT_EQUAL_HEX32(expected[n - 1], val);
    TEST_ASSERT_EQUAL_size_t(sizeof(fixture), c.pos);
  }
}

void test_step_advances_pos(void) {
  const uint8_t bvlc[] = {0x81, 0x0B, 0x00, 0x08};
  uint32_t type, function, length;
  c = make_cursor(bvlc, sizeof(bvlc));
  TEST_ASSERT_EQUAL_INT(OK, c_select(&type, &c, 1, 1));
  TEST_ASSERT_EQUAL_INT(OK, c_select(&function, &c, 1, 1));
  TEST_ASSERT_EQUAL_INT(OK, c_select(&length, &c, 2, 2));
  TEST_ASSERT_EQUAL_HEX32(0x81, type);
  TEST_ASSERT_EQUAL_HEX32(0x0B, function);
  TEST_ASSERT_EQUAL_HEX32(0x08, length);
  TEST_ASSERT_EQUAL_size_t(4, c.pos);
}

void test_step_zero_stays_in_place(void) {
  uint32_t first = 0;
  uint32_t second = 0;
  TEST_ASSERT_EQUAL_INT(OK, c_select(&first, &c, 2, 0));
  TEST_ASSERT_EQUAL_INT(OK, c_select(&second, &c, 2, 0));
  TEST_ASSERT_EQUAL_HEX32(first, second);
  TEST_ASSERT_EQUAL_size_t(0, c.pos);
}

void test_step_can_skip_bytes(void) {
  uint32_t val = 0xDEADBEEF;
  TEST_ASSERT_EQUAL_INT(OK, c_select(&val, &c, 1, 3));
  TEST_ASSERT_EQUAL_size_t(3, c.pos);
  TEST_ASSERT_EQUAL_INT(OK, c_select(&val, &c, 1, 0));
  TEST_ASSERT_EQUAL_HEX32(0x34, val);
}

void test_step_can_differ_from_n(void) {
  uint32_t val = 0xDEADBEEF;
  TEST_ASSERT_EQUAL_INT(OK, c_select(&val, &c, 2, 1));
  TEST_ASSERT_EQUAL_HEX32(0x21F4, val);
  TEST_ASSERT_EQUAL_INT(OK, c_select(&val, &c, 2, 1));
  TEST_ASSERT_EQUAL_HEX32(0xF423, val);
}

void test_reject_step_past_end(void) {
  const size_t steps[] = {2, 5, (size_t)-1};
  for (size_t i = 0; i < 3; i++) {
    uint32_t val = 0xDEADBEEF;
    c.pos = 3;
    TEST_ASSERT_EQUAL_INT(END, c_select(&val, &c, 1, steps[i]));
    TEST_ASSERT_EQUAL_HEX32(0xDEADBEEF, val);
    TEST_ASSERT_EQUAL_size_t(3, c.pos);
  }
}

void test_reject_5_byte(void) {
  uint32_t val = 0xDEADBEEF;
  TEST_ASSERT_EQUAL_INT(ERR, c_select(&val, &c, 5, 0));
  TEST_ASSERT_EQUAL_HEX32(0xDEADBEEF, val);
}

void test_reject_neg_byte(void) {
  uint32_t val = 0xDEADBEEF;
  TEST_ASSERT_EQUAL_INT(ERR, c_select(&val, &c, -1, 0));
  TEST_ASSERT_EQUAL_HEX32(0xDEADBEEF, val);
}

void test_reject_zero_byte(void) {
  uint32_t val = 0xDEADBEEF;
  TEST_ASSERT_EQUAL_INT(ERR, c_select(&val, &c, 0, 0));
  TEST_ASSERT_EQUAL_HEX32(0xDEADBEEF, val);
}

void test_reject_one_byte_short(void) {
  for (size_t n = 1; n <= 4; n++) {
    uint32_t val = 0xDEADBEEF;
    size_t pos = sizeof(fixture) - (n - 1);
    c.pos = pos;
    TEST_ASSERT_EQUAL_INT(END, c_select(&val, &c, n, 0));
    TEST_ASSERT_EQUAL_HEX32(0xDEADBEEF, val);
    TEST_ASSERT_EQUAL_size_t(pos, c.pos);
  }
}

void test_reject_empty_buffer(void) {
  c = make_cursor(fixture, 0);
  for (size_t n = 1; n <= 4; n++) {
    uint32_t val = 0xDEADBEEF;
    TEST_ASSERT_EQUAL_INT(END, c_select(&val, &c, n, 0));
    TEST_ASSERT_EQUAL_HEX32(0xDEADBEEF, val);
  }
}

void test_fixture_read(void) {
  uint32_t val = 0x99;
  uint8_t val_a[5] = {0};
  uint8_t status[5] = {0};
  uint8_t status_expected[5] = {OK, OK, OK, OK, END};
  memset(status, 0x99, sizeof(status));
  memset(val_a, 0x99, sizeof(val_a));
  for (int i = 0; i < 5; i++) {
    status[i] = c_select(&val, &c, 1, 1);
    if (status[i] == END) {
      break;
    }
    val_a[i] = (uint8_t)val;
  }
  for (int i = 0; i < 4; i++) {
    TEST_ASSERT_EQUAL_HEX8(fixture[i], val_a[i]);
  }

  for (int i = 0; i < 5; i++) {
    TEST_ASSERT_EQUAL_INT(status_expected[i], status[i]);
  }
}

void test_fixture_read_last(void) {
  uint32_t val = 0x99;
  c.pos = 3;
  TEST_ASSERT_EQUAL_INT(OK, c_select(&val, &c, 1, 1));
  TEST_ASSERT_EQUAL_HEX32(0x34, val);
  TEST_ASSERT_EQUAL_INT(END, c_select(&val, &c, 1, 1));
}

void test_fixture_reject_cursor_end(void) {
  for (size_t n = 1; n <= 4; n++) {
    uint32_t val = 0xDEADBEEF;
    c.pos = 5;
    TEST_ASSERT_EQUAL_INT(END, c_select(&val, &c, n, 0));
    TEST_ASSERT_EQUAL_HEX32(0xDEADBEEF, val);
  }
}

void test_fixture_reject_bad_cursor(void) {
  for (size_t n = 1; n <= 4; n++) {
    uint32_t val = 0xDEADBEEF;
    c.pos = -1;
    for (int i = 0; i < 5; i++) { // -1, -2, -3, -4, -5
      TEST_ASSERT_EQUAL_INT(END, c_select(&val, &c, n, 0));
      TEST_ASSERT_EQUAL_HEX32(0xDEADBEEF, val);
      c.pos--;
    }
  }
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_reads_one_byte);
  RUN_TEST(test_reads_two_byte);
  RUN_TEST(test_reads_three_byte);
  RUN_TEST(test_reads_four_byte);
  RUN_TEST(test_reads_high_bit_bytes);
  RUN_TEST(test_reads_exact_fit_at_end);
  RUN_TEST(test_step_advances_pos);
  RUN_TEST(test_step_zero_stays_in_place);
  RUN_TEST(test_step_can_skip_bytes);
  RUN_TEST(test_step_can_differ_from_n);
  RUN_TEST(test_reject_step_past_end);
  RUN_TEST(test_reject_5_byte);
  RUN_TEST(test_reject_neg_byte);
  RUN_TEST(test_reject_zero_byte);
  RUN_TEST(test_reject_one_byte_short);
  RUN_TEST(test_reject_empty_buffer);
  RUN_TEST(test_fixture_read);
  RUN_TEST(test_fixture_read_last);
  RUN_TEST(test_fixture_reject_cursor_end);
  RUN_TEST(test_fixture_reject_bad_cursor);
  return UNITY_END();
}
