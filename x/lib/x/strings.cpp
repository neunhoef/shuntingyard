// Translated from strings.x

#include <string.h>
#include <stdlib.h>

#include "errors.h"
#include "strings.h"

extern "C" {

void x__init__empty(x__strings__string* s) {
  s->buf = nullptr;
  s->alloc = 0;
  s->size = 0;
}

void x__init__allocSize(x__strings__string* s, size_t allocSize) {
  s->buf = (uint8_t*) malloc(allocSize);
  if (s->buf == nullptr) {
    s->alloc = 0;
  } else {
    s->alloc = allocSize;
  }
  s->size = 0;
}

void x__init__ptrSize(x__strings__string* s, uint8_t* cs, size_t len) {
  s->buf = (uint8_t*) malloc(len);
  if (s->buf == nullptr) {
    s->alloc = 0;
    s->size = 0;
  } else {
    memcpy(s->buf, cs, len);
    s->alloc = len;
    s->size = len;
  }
}

void x__init__cstring(x__strings__string* s, uint8_t* cs) {
  size_t len = strlen((char*) cs);
  x__init__ptrSize(s, cs, len);
}

void x__strings__init__copy(x__strings__string* s, x__strings__string* t) {
  x__init__ptrSize(s, t->buf, t->size);
}

void x__strings__exit(x__strings__string* s) {
  free(s->buf);
  s->buf = nullptr;
  s->alloc = 0;
  s->size = 0;
}

void x__strings__set__empty(x__strings__string* s) {
  free(s->buf);
  s->buf = nullptr;
  s->alloc = 0;
  s->size = 0;
}

void x__strings__set__ptrSize(x__strings__string* s, uint8_t const* const m, size_t len) {
  if (s->alloc >= len && s->alloc <= 2 * len) {
    memcpy(s->buf, m, len);
    s->size = len;
  } else {
    uint8_t* tmp = (uint8_t*) realloc(s->buf, len);
    if (tmp == nullptr) {
      x__errors__set__msg(&x__errors__err, x__errors__ALLOCATION_FAILED,
                          x__errors__ALLOCATION_FAILED_MSG);
    } else {
      s->buf = tmp;
      memcpy(s->buf, m, len);
    }
  }
}

uint8_t x__strings__at_raw(x__strings__string* s, size_t pos) {
  return s->buf[pos];
}

uint8_t x__strings__at(x__strings__string* s, size_t pos) {
  if (pos < s->size) {
    return s->buf[pos];
  } else {
    x__errors__set__msg(&x__errors__err, x__errors__OUT_OF_BOUNDS,
                        x__errors__OUT_OF_BOUNDS_MSG);
    return 0;
  }
}

bool x__strings__reserve(x__strings__string* s, size_t size);

bool x__strings__append__string(x__strings__string* s, x__strings__string* t);
bool x__strings__append__char(x__strings__string* s, uint8_t c);
bool x__strings__append__chars(x__strings__string* s, uint8_t const* const t, size_t si);

}

