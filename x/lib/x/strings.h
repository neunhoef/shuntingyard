// Translated from strings.x

#ifndef X_STRINGS_H
#define X_STRINGS_H 1

#include <cstdint>

extern "C" {

struct x__strings__string {
  uint8_t *buf;
  size_t  alloc;
  size_t  size;
};

void x__strings__init__empty(x__strings__string* s);
void x__strings__init__allocSize(x__strings__string* s, size_t allocSize);
void x__strings__init__ptrSize(x__strings__string* s, uint8_t const* const cs, size_t len);
void x__strings__init__cstring(x__strings__string* s, uint8_t const* const cs);
void x__strings__init__copy(x__strings__string* s, x__strings__string* t);

void x__strings__exit(x__strings__string* s);

void x__strings__set__empty(x__strings__string* s);
void x__strings__set__ptrSize(x__strings__string* s, uint8_t const* const m, size_t len);

uint8_t x__strings__at_raw(x__strings__string* s, size_t pos);
uint8_t x__strings__at(x__strings__string* s, size_t pos);

bool x__strings__reserve(x__strings__string* s, size_t size);

bool x__strings__append__string(x__strings__string* s, x__strings__string* t);
bool x__strings__append__char(x__strings__string* s, uint8_t c);
bool x__strings__append__chars(x__strings__string* s, uint8_t const* const t, size_t si);

}

#endif
