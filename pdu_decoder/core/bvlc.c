// WARNING: Unwrappers assume ptr_data points to a complete header!

#include "bvlc.h"
#include "cursor.h"
#include <stdint.h>

UnwrappedBVLCPkt make_bvlc(const uint8_t *start, size_t len, size_t funct) {
  return (UnwrappedBVLCPkt){.ptr_data = start, .funct = funct, .len = len};
}

Result bvlc_unwrap(UnwrappedBVLCPkt *out, const uint8_t *ptr_data) {

  if (out == NULL || ptr_data == NULL) {
    return BVLC_ERR;
  }

  // Get Length: Octets 2-3, BE
  size_t len = ptr_data[2];
  size_t len_low = ptr_data[3];
  len = (len << 8) | len_low;

  if (len > BVLC_MAX_LEN || len < BVLC_MIN_LEN) {
    return BVLC_ERR;
  }

  Cursor c = make_cursor(ptr_data, len);

  uint32_t results[3] = {2};
  for (int i = 0; i < 2; i++) {
    // 0 -> type
    // 1 -> funct
    c_select(&results[i], &c, 1, 1);
  }

  // Drop packet if not type BACnet/IP
  if (results[0] != BVLC_TYPE) {
    return BVLC_DROP;
  }

  // Data is at start + 4
  const uint8_t *start = ptr_data + 4;
  *out = make_bvlc(start, len, results[1]);

  return BVLC_OK;
}
