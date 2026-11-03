#include "bvlc.h"

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  // NOTE: The higher level does this check, not bvlc_unwrap!
  if (size < 4) {
    return 0;
  }
  UnwrappedBVLCPkt pkt;
  bvlc_unwrap(&pkt, data);
  return 0;
}
