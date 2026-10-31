#include "cursor.h"

Cursor make_cursor(const uint8_t *data, size_t len) {
  return (Cursor){.ptr_buf = data, .len = len, .pos = 0};
}

static void read_u8(uint32_t *out, const Cursor *c) {
  *out = c->ptr_buf[c->pos]; // A
}

static void read_u16(uint32_t *out, const Cursor *c) {
  uint32_t high = c->ptr_buf[c->pos];    // A
  uint32_t low = c->ptr_buf[c->pos + 1]; // B
  *out = (high << 8) | low;              // A + B
}

static void read_u32(uint32_t *out, const Cursor *c, size_t n) {
  uint32_t value = c->ptr_buf[c->pos];           // A
  value = (value << 8) | c->ptr_buf[c->pos + 1]; // AB
  value = (value << 8) | c->ptr_buf[c->pos + 2]; // ABC
  if (n == 4) {
    value = (value << 8) | c->ptr_buf[c->pos + 3]; // ABCD
  }
  *out = value;
}

State c_select(uint32_t *out, Cursor *c, size_t n, size_t step) {
  if (n == 0 || n > 4) {
    return ERR;
  }
  if (c->pos > c->len) {
    return END;
  }
  size_t left = c->len - c->pos;
  if (left < n || left < step) {
    return END;
  }
  switch (n) {
  case 1:
    read_u8(out, c);
    break;
  case 2:
    read_u16(out, c);
    break;
  default:
    read_u32(out, c, n);
    break;
  }
  c->pos += step;
  return OK;
}
