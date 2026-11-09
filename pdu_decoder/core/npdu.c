// WARNING: NPDU unwrapper assumes:
//  1. Valid Unwrapped BVLC
//  2. NPDU containing packet
#include "npdu.h"
#include "bvlc.h"
#include "cursor.h"

#define OFFSET_VERSION 0
#define OFFSET_CNTLBYTE 1
#define OFFSET_EOFCNTL (OFFSET_CNTLBYTE + 1)
#define OFFSET_DNET (OFFSET_CNTLBYTE + 2)
#define SIZE_DEST (16 + 8 + 8) // dnet, dlen, dadr

// Create a src addr struct
NpduSrcAddr _make_src_addr(int16_t snet, int8_t slen, int8_t sadr) {
  return (NpduSrcAddr){.snet = snet, .slen = slen, .sadr = sadr};
}

// Create a dest address struct
NpduDestAddr _make_dest_addr(int16_t dnet, int8_t dlen, int8_t dadr,
                             int16_t hop_count) {
  return (NpduDestAddr){
      .dnet = dnet, .dlen = dlen, .dadr = dadr, .hop_count = hop_count};
}

// Create a network message struct
NpduNetMsg _make_net_msg(int16_t type, int8_t *data, int16_t vendor_id) {
  return (NpduNetMsg){.type = type, .data = data, .vendor_id = vendor_id};
}

// Create a unwrapped packet
UnwrappedNPDUPkt _make_unwrapped_pkt(NpduDestAddr npdu_dest,
                                     NpduSrcAddr npdu_src, NpduNetMsg npdu_msg,
                                     uint8_t cntl) {
  return (UnwrappedNPDUPkt){.npdu_dest = npdu_dest,
                            .npdu_src = npdu_src,
                            .npdu_netmsg = npdu_msg,
                            .cntl = cntl};
}

NPDUResult npdu_unwrap(UnwrappedNPDUPkt *out,
                       const UnwrappedBVLCPkt *ptr_bvlc) {
  if (ptr_bvlc == NULL || ptr_bvlc->ptr_data == NULL) {
    return NPDU_ERR;
  }

  // Funct must carry NPDU
  for (size_t i = 0; i < (sizeof(NPDU_FUNCTS) / sizeof(NPDU_FUNCTS[0])); i++) {
    if (ptr_bvlc->funct == NPDU_FUNCTS[i]) {
      break;
    }
    return NPDU_DROP; // BUG: put this drop after loop
  };

  // Check Version
  if (ptr_bvlc->ptr_data[OFFSET_VERSION] != 0x01) {
    return NPDU_ERR;
  }

  // Parse Order: TODO

  // Check If dest
  if (ptr_bvlc->ptr_data[OFFSET_CNTLBYTE] & DEST) {
    Cursor c_dest = {&ptr_bvlc->ptr_data[OFFSET_DNET], SIZE_DEST, 0};
    uint32_t slice[3] = {0};
    slice[0] = c_select(slice, &c_dest, 2, 1); // Read
  }
}

// TODO: Impl this parser to read the cntl offset
