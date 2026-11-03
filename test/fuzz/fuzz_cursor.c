#include "cursor.h"
#include <stdlib.h>

// The first half of the input picks n and step, the second half is the packet
int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  size_t ops = size / 2;
  Cursor c = make_cursor(data + ops, size - ops);

  for (size_t i = 0; i < ops; i++) {
    size_t n = data[i] & 0x07;
    size_t step = data[i] >> 3;
    size_t pos = c.pos;
    uint32_t val = 0xDEADBEEF;

    State s = c_select(&val, &c, n, step);

    if (s == C_OK && c.pos != pos + step) {
      abort();
    }
    if (s != C_OK && (c.pos != pos || val != 0xDEADBEEF)) {
      abort();
    }
    if (c.pos > c.len) {
      abort();
    }
  }
  return 0;
}
