#include "unity.h"
#include <bvlc.h>
#include <stdint.h>

// Type -> 0x81 (BACnet/IP)
// Funct -> 0xFF
// Size -> 0x00, 0x06, 6 bytes (incl header)
// Data -> 0x02, 0x01
static const uint8_t fixture[] = {0x81, 0xFF, 0x00, 0x06, 0x02, 0x01};

void tearDown(void) {}
void setUp(void) {}

void test_reads_valid_packet(void) {
  UnwrappedBVLCPkt pkt;
  Result r = bvlc_unwrap(&pkt, fixture);
  TEST_ASSERT_EQUAL_INT(BVLC_OK, r);
  TEST_ASSERT_EQUAL_INT(0x06, pkt.len);
  TEST_ASSERT_EQUAL_INT(0xFF, pkt.funct);
  size_t total = 0;
  for (size_t i = 0; i < pkt.len - 4; i++) {
    total = pkt.ptr_data[i] + total;
  }
  // Data total should be 3
  TEST_ASSERT_EQUAL_INT(0x03, total);
}

void test_reject_oversized_packet(void) {
  UnwrappedBVLCPkt pkt;
  // [2] and [3] should reflect the u16 max of 65535
  uint8_t fixture[] = {0x81, 0xFF, 0xFF, 0xFF, 0x02, 0x01};
  Result r = bvlc_unwrap(&pkt, fixture);
  TEST_ASSERT_EQUAL_INT(BVLC_ERR, r);
}

void test_reject_undersized_packet(void) {
  UnwrappedBVLCPkt pkt;
  uint8_t fixture[] = {0x81, 0xFF, 0x00, 0x00, 0x02, 0x01};
  uint8_t results[5] = {99};
  for (size_t i = 0; i < 5; i++) {
    results[i] = bvlc_unwrap(&pkt, fixture);
    fixture[3]++; // Inc fixture, last iter should make fixture OK
  }
  // First 4 results should be err
  for (size_t i = 0; i < 4; i++) {
    TEST_ASSERT_EQUAL_INT(BVLC_ERR, results[i]);
  }
  // Last result should be OK
  TEST_ASSERT_EQUAL_INT(BVLC_OK, results[4]);
}

void test_drop_non_bvlc_packet(void) {
  // 0x82 is BACnet/IPv6
  uint8_t fixture[] = {0x82, 0xFF, 0x00, 0x06, 0x02, 0x01};
  UnwrappedBVLCPkt pkt;
  Result r = bvlc_unwrap(&pkt, fixture);
  TEST_ASSERT_EQUAL_INT(BVLC_DROP, r);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_reads_valid_packet);
  RUN_TEST(test_reject_oversized_packet);
  RUN_TEST(test_reject_undersized_packet);
  return UNITY_END();
}
