// Translated from strings.x

#include <string.h>
#include <stdlib.h>

#include "errors.h"
#include "strings.h"

extern "C" {

void x_init_empty(x_strings_string& s) {
  s.buf = nullptr;
  s.alloc = 0;
  s.size = 0;
}

void x_init_allocSize(x_strings_string& s, size_t allocSize) {
  s.buf = (uint8_t*) malloc(allocSize);
  if (s.buf == nullptr) {
    s.alloc = 0;
  } else {
    s.alloc = allocSize;
  }
  s.size = 0;
}

void x_init_ptrSize(x_strings_string& s, uint8_t* cs, size_t len) {
  s.buf = (uint8_t*) malloc(len);
  if (s.buf == nullptr) {
    s.alloc = 0;
    s.size = 0;
  } else {
    memcpy(s.buf, cs, len);
    s.alloc = len;
    s.size = len;
  }
}

void x_init_cstring(x_strings_string& s, uint8_t* cs) {
  size_t len = strlen((char*) cs);
  x_init_ptrSize(s, cs, len);
}

void x_strings_init_copy(x_strings_string& s, x_strings_string& t) {
  x_init_ptrSize(s, t.buf, t.size);
}

void x_strings_exit(x_strings_string& s) {
  free(s.buf);
  s.buf = nullptr;
  s.alloc = 0;
  s.size = 0;
}

void x_strings_set_empty(x_strings_string& s) {
  free(s.buf);
  s.buf = nullptr;
  s.alloc = 0;
  s.size = 0;
}

void x_strings_set_ptrSize(x_strings_string& s, uint8_t const* const m, size_t len) {
  if (s.alloc >= len && s.alloc <= 2 * len) {
    memcpy(s.buf, m, len);
    s.size = len;
  } else {
    uint8_t* tmp = (uint8_t*) realloc(s.buf, len);
    if (tmp == nullptr) {
      x_errors_set_msg(x_errors_err, x_errors_ALLOCATION_FAILED,
                       x_errors_ALLOCATION_FAILED_MSG);
    } else {
      s.buf = tmp;
      memcpy(s.buf, m, len);
    }
  }
}

uint8_t x_strings_at_raw(x_strings_string& s, size_t pos) {
  return s.buf[pos];
}

uint8_t x_strings_at(x_strings_string& s, size_t pos) {
  if (pos < s.size) {
    return s.buf[pos];
  } else {
    x_errors_set_msg(x_errors_err, x_errors_OUT_OF_BOUNDS,
                     x_errors_OUT_OF_BOUNDS_MSG);
    return 0;
  }
}

bool x_strings_reserve(x_strings_string& s, size_t size);

bool x_strings_append_string(x_strings_string& s, x_strings_string const& t);
bool x_strings_append_char(x_strings_string& s, uint8_t c);
bool x_strings_append_chars(x_strings_string& s, uint8_t const* const t, size_t si);

}

