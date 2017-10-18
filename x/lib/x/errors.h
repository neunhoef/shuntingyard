// Translated from error.x

#include <cstdint>

#include "strings.h"

extern "C" {

struct x__errors__error {
  int32_t code;
  uint32_t len;
  unsigned char msg[96];
  x__strings__string longMsg;
  void* data;
};

extern thread_local x__errors__error x__errors__err;

// Constructors:

void x__errors__init__empty(x__errors__error* e);
void x__errors__init__msg(x__errors__error* e, int32_t c, uint8_t const* const m);

// Setters:

void x__errors__set__empty(x__errors__error* e);
void x__errors__set__msg(x__errors__error* e, int32_t c, uint8_t const* const m);

// Destructor:

void x__errors__exit(x__errors__error* e);

// Test if no x__errors__error occurred:

bool x__errors__ok(x__errors__error* e);

// Test if an error occurred:

bool x__errors__bool(x__errors__error* e);

// Return the error code:

int32_t x__errors__code(x__errors__error* e);

// Retrieve the error message:

void x__errors__msg(x__errors__error* e, x__strings__string* res);

// Error codes:

extern int32_t const x__errors__OUT_OF_BOUNDS;
extern uint8_t const* const x__errors__OUT_OF_BOUNDS_MSG;
extern int32_t const x__errors__ALLOCATION_FAILED;
extern uint8_t const* const x__errors__ALLOCATION_FAILED_MSG;

}
