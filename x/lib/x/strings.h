// Translated from strings.x

#ifndef X_STRINGS_H
#define X_STRINGS_H 1

#include <cstdint>
#include <stdlib.h>

extern "C" {

struct x_strings_string {
  uint8_t *buf;
  size_t  alloc;
  size_t  size;
};

void x_strings_init_empty(x_strings_string& s);
void x_strings_init_allocSize(x_strings_string& s, size_t allocSize);
void x_strings_init_ptrSize(x_strings_string& s, uint8_t const* const cs, size_t len);
void x_strings_init_cstring(x_strings_string& s, uint8_t const* const cs);
void x_strings_init_copy(x_strings_string& s, x_strings_string& t);

void x_strings_exit(x_strings_string& s);

void x_strings_set_empty(x_strings_string& s);
void x_strings_set_ptrSize(x_strings_string& s, uint8_t const* const m, size_t len);

uint8_t x_strings_at_raw(x_strings_string& s, size_t pos);
uint8_t x_strings_at(x_strings_string& s, size_t pos);

void x_strings_reserve(x_strings_string& s, size_t size);

void x_strings_append_string(x_strings_string& s, x_strings_string const& t);
void x_strings_append_char(x_strings_string& s, uint8_t c);
void x_strings_append_chars(x_strings_string& s, uint8_t const* const t, size_t si);

}

#endif
