// Translated from error.x

#include <string.h>

#include "errors.h"

extern "C" {

thread_local x__errors__error x__errors__err;

// Constructors:

void x__errors__init__empty(x__errors__error* e) {
  x__strings__init__empty(&e->longMsg);
  x__errors__set__empty(e);
}

void x__errors__init__msg(x__errors__error* e, int32_t c, uint8_t const* const m) {
  x__strings__init__empty(&e->longMsg);
  x__errors__set__msg(e, c, m);
}

// Setters:

void x__errors__set__empty(x__errors__error* e) {
  e->code = 0;
  e->len = 0;
  x__strings__set__empty(&e->longMsg);
  e->data = nullptr;
}

void x__errors__set__msg(x__errors__error* e, int32_t c, uint8_t const* const m) {
  e->code = c;
  size_t size = strlen((char*) m);
  if (size <= 96) {
    e->len = (uint32_t) size;
    memcpy(e->msg, m, size);
    x__strings__set__empty(&e->longMsg);
  } else {
    e->len = 0;
    x__strings__set__ptrSize(&e->longMsg, m, size);
  }
  e->data = nullptr;
}

// Destructor:

void x__errors__exit(x__errors__error* e) {
  x__strings__exit(&e->longMsg);
}

// Test if no error occurred:

bool x__errors__ok(x__errors__error* e) {
  return (e->code == 0);
};

// Test if an error occurred:

bool x__errors__bool(x__errors__error* e) {
  return e->code != 0;
};

// Return the error code:

int32_t x__errors__code(x__errors__error* e) {
  return e->code;
};

// Retrieve the error message:

void x__errors__msg(x__errors__error* e, x__strings__string* res) {
  if (e->code == 0) {
    x__strings__init__empty(res);
  } else if (e->len == 0) { 
    x__strings__init__copy(res, &e->longMsg);
  } else {
    x__strings__init__ptrSize(res, e->msg, e->len);
  }
};

// Error codes:

int32_t const x__errors__OUT_OF_BOUNDS = 1;
uint8_t const* const x__errors__OUT_OF_BOUNDS_MSG 
  = (uint8_t const*) "out of bounds";
int32_t const x__errors__ALLOCATION_FAILED = 2;
uint8_t const* const x__errors__ALLOCATION_FAILED_MSG
  = (uint8_t const*) "allocation failed";

}
