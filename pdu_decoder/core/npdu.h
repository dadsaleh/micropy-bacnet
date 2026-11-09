#include <stdint.h>
// Network Protocol Data Unit (NPUD) controlls routing between BACnet networks.
//
// Because there is a dependancy on whatever the function value of the BVLC is,
// the caller should only parse a NPUD if the function is of:
// 0x09
// 0x0A
// 0x0B
//
// Otherwise, the packet is dropped with "NDPU_DROP" as its result

// Control Byte field Map:
//
// RESERVED_1 & 2 -> Must be Zero
// NETMSG -> 1 = Netowork Message, 0 -> APDU Follows
// DEST -> IF 1, DNET, DLEN, DADR and Hop Count follow in that order
// SOURCE -> IF 1, SNET, SLEN, and SADR follow in that order
// SENDER_REPLY -> IF 1 sender expects a ack
#define NETMSG 0x80
#define RESERVED_1 0x40
#define DEST 0x20
#define RESERVED_2 0x10
#define SOURCE 0x04
#define SENDER_REPLY 0x02

// Prioirty:
//
// 00 -> Normal
// 01 -> Urgent
// 10 -> Critical
// 11 -> Life Safety
#define PRIOITY_HIGH 0x01
#define PRIOITY_LOW 0x00

// Only Valid Functions that carry NPDU
static const uint8_t NPDU_FUNCTS[] = {0x04, 0x09, 0x0A, 0x0B};

// Destination Specifier Struct
typedef struct {
  int16_t dnet;
  int8_t dlen;
  int8_t dadr;
  int16_t hop_count;
} NpduDestAddr;

// Source Specifier Struct
typedef struct {
  int16_t snet;
  int8_t slen;
  int8_t sadr;
} NpduSrcAddr;

// Network Message
typedef struct {
  int16_t type;
  int8_t *data;      // NOTE: Varies based on msg type
  int16_t vendor_id; // NOTE: Vendor ID only if type greater than 0x80
} NpduNetMsg;

// Unwrapped NPDU packet, use the control to decide what fields are valid
typedef struct {
  NpduDestAddr npdu_dest;
  NpduSrcAddr npdu_src;
  NpduNetMsg npdu_netmsg;
  int8_t cntl; // Control Byte
} UnwrappedNPDUPkt;

typedef enum {
  NPDU_OK,
  NPDU_DROP,
  NPDU_ERR,
} NPDUResult;
