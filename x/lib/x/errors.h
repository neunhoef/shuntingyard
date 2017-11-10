// Translated from error.x

#ifndef X_ERRORS_H
#define X_ERRORS_H 1

#include <cstdint>

#include "strings.h"

extern "C" {

struct x_errors_error {
  int32_t code;
  uint32_t len;
  unsigned char msg[96];
  x_strings_string longMsg;
  void* data;
};

extern thread_local x_errors_error x_errors_err;

// Constructors:

void x_errors_init_empty(x_errors_error& e);
void x_errors_init_msg(x_errors_error& e, int32_t c, uint8_t const* const m);

// Setters:

void x_errors_set_empty(x_errors_error& e);
void x_errors_set_msg(x_errors_error& e, int32_t c, uint8_t const* const m);

// Destructor:

void x_errors_exit(x_errors_error& e);

// Test if no x_errors_error occurred:

bool x_errors_ok(x_errors_error& e);

// Test if an error occurred:

bool x_errors_bool(x_errors_error& e);

// Return the error code:

int32_t x_errors_code(x_errors_error& e);

// Retrieve the error message:

void x_errors_msg(x_errors_error& e, x_strings_string& res);

// Error codes:

extern int32_t const x_errors_OUT__OF__BOUNDS;
extern uint8_t const* const x_errors_OUT__OF__BOUNDS__MSG;
extern int32_t const x_errors_ALLOCATION__FAILED;
extern uint8_t const* const x_errors_ALLOCATION__FAILED__MSG;

}

#endif
