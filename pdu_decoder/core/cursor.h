#ifndef CURSOR_H
#define CURSOR_H

#include <stddef.h>
#include <stdint.h>

typedef enum { OK, END, ERR } State;

typedef struct {
  const uint8_t *ptr_buf;
  size_t len;
  size_t pos;
} Cursor;

/** Make a cursor over a packet. The bytes are not copied or validated, so data
 * must stay valid for as long as the cursor is used. */
Cursor make_cursor(const uint8_t *data, size_t len);

/** Read n bytes, from 1 to 4, as big-endian into out, then add step to pos.
 * Use a step of 0 to stay in place, or n to move past what was read. Returns
 * ERR for a bad n, or END if fewer than n bytes are left or step would pass the
 * end. On failure, out and pos are left unchanged. */
State c_select(uint32_t *out, Cursor *c, size_t n, size_t step);

#endif
