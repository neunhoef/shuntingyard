// Translated from strings.x

#include <string.h>
#include <stdlib.h>

#include "errors.h"
#include "strings.h"

extern "C" {

void x_strings_init_empty(x_strings_string& s) {
  s.buf = nullptr;
  s.alloc = 0;
  s.size = 0;
}

void x_strings_init_allocSize(x_strings_string& s, size_t allocSize) {
  s.buf = (uint8_t*) malloc(allocSize);
  if (s.buf == nullptr) {
    s.alloc = 0;
  } else {
    s.alloc = allocSize;
  }
  s.size = 0;
}

void x_strings_init_ptrSize(x_strings_string& s, uint8_t const* const cs, size_t len) {
  s.buf = (uint8_t*) malloc(len+1);
  if (s.buf == nullptr) {
    s.alloc = 0;
    s.size = 0;
  } else {
    memcpy(s.buf, cs, len);
    s.buf[len] = 0;
    s.alloc = len;
    s.size = len;
  }
}

void x_strings_init_string(x_strings_string& s, x_strings_string const& t) {
  x_strings_init_ptrSize(s, t.buf, t.size);
}

void x_strings_init_cstring(x_strings_string& s, uint8_t const* const cs) {
  size_t len = strlen((char*) cs);
  x_strings_init_ptrSize(s, cs, len);
}

void x_strings_init_copy(x_strings_string& s, x_strings_string& t) {
  x_strings_init_ptrSize(s, t.buf, t.size);
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
  if (s.alloc >= len+1 && s.alloc <= 2 * len) {
    memcpy(s.buf, m, len);
    s.buf[len] = 0;
    s.size = len;
  } else {
    uint8_t* tmp = (uint8_t*) realloc(s.buf, len+1);
    if (tmp == nullptr) {
      x_errors_set_msg(x_errors_err, x_errors_ALLOCATION__FAILED,
                       x_errors_ALLOCATION__FAILED__MSG);
    } else {
      s.buf = tmp;
      memcpy(s.buf, m, len);
      s.buf[len] = 0;
      s.size = len;
    }
  }
}

uint8_t x_strings_at_raw(x_strings_string& s, size_t pos) {
  return s.buf[pos];
}

uint8_t x_strings_at(x_strings_string& s, size_t pos) {
  if (pos <= s.size) {
    return s.buf[pos];
  } else {
    x_errors_set_msg(x_errors_err, x_errors_OUT__OF__BOUNDS,
                     x_errors_OUT__OF__BOUNDS__MSG);
    return 0;
  }
}

void x_strings_reserve(x_strings_string& s, size_t size) {
  if (s.alloc < size+1) {
    size_t newSize = 2 * s.alloc;
    if (newSize < size+1) {
      newSize = size+1;
    }
    uint8_t* newbuf = (uint8_t*) realloc(s.buf, newSize);
    if (newbuf == nullptr) {
      x_errors_set_msg(x_errors_err, x_errors_ALLOCATION__FAILED,
                       x_errors_ALLOCATION__FAILED__MSG);
    } {
      s.buf = newbuf;
      s.alloc = newSize;
    }
  }
}

void x_strings_append_string(x_strings_string& s, x_strings_string const& t) {
  x_strings_reserve(s, s.size + t.size);
  if (x_errors_bool(x_errors_err)) { return; }
  memcpy(s.buf + s.size, t.buf, t.size);
  s.size += t.size;
  s.buf[s.size] = 0;
}

void x_strings_append_char(x_strings_string& s, uint8_t c) {
  x_strings_reserve(s, s.size + 1);
  if (x_errors_bool(x_errors_err)) { return; }
  s.buf[s.size] = c;
  s.size += 1;
  s.buf[s.size] = 0;
}

void x_strings_append_chars(x_strings_string& s, uint8_t const* const t,
                            size_t si) {
  x_strings_reserve(s, s.size + si);
  if (x_errors_bool(x_errors_err)) { return; }
  memcpy(s.buf + s.size, t, si);
  s.size += si;
  s.buf[s.size] = 0;
}

}
