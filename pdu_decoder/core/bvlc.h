#ifndef BVLC_H
#define BVLC_H

#include <stddef.h>
#include <stdint.h>

// BVLC Header:
// Octet 0: Type field
// Octet 1: Function
// Octets 2-3: Length, including the header
//
// The Type must be 0x81 for BACnet/IP. Anything else is dropped by returning
// the BVLC_DROP enum up the call chain, then through the FFI to the Python
// caller as the tagged constant mp_const_none.
#define BVLC_TYPE 0x81

// Max "reasonable" length of a BVLC packet:
// 4 octets for the header
// 6 octets for the Forwarded-NPDU address
// Length can reach about 1507, as it counts the entire message
//
// Returns an error if "len" is larger than 1536
#define BVLC_MAX_LEN 1536
#define BVLC_MIN_LEN 4

// ptr_data: Pointer to the unwrapped data, excluding the header
// len: Total size decoded from octets 2 and 3 of the header
// funct: Function code
typedef struct {
  const uint8_t *ptr_data;
  size_t len;
  size_t funct;
} UnwrappedBVLCPkt;

// Return statuses
typedef enum {
  BVLC_OK,
  BVLC_ERR,
  BVLC_DROP,
} Result;

Result bvlc_unwrap(UnwrappedBVLCPkt *out, const uint8_t *ptr_data);

// This module and others like it do NOT validate funct or any NPDU messages.
// Those checks happen above the C parsers, in the Python logic.

#endif
