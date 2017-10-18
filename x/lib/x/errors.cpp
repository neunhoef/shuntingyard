// Translated from error.x

#include <string.h>

#include "errors.h"

extern "C" {

thread_local x_errors_error x_errors_err;

// Constructors:

void x_errors_init_empty(x_errors_error& e) {
  x_strings_init_empty(e.longMsg);
  x_errors_set_empty(e);
}

void x_errors_init_msg(x_errors_error& e, int32_t c, uint8_t const* const m) {
  x_strings_init_empty(e.longMsg);
  x_errors_set_msg(e, c, m);
}

// Setters:

void x_errors_set_empty(x_errors_error& e) {
  e.code = 0;
  e.len = 0;
  x_strings_set_empty(e.longMsg);
  e.data = nullptr;
}

void x_errors_set_msg(x_errors_error& e, int32_t c, uint8_t const* const m) {
  e.code = c;
  size_t size = strlen((char*) m);
  if (size <= 96) {
    e.len = (uint32_t) size;
    memcpy(e.msg, m, size);
    x_strings_set_empty(e.longMsg);
  } else {
    e.len = 0;
    x_strings_set_ptrSize(e.longMsg, m, size);
  }
  e.data = nullptr;
}

// Destructor:

void x_errors_exit(x_errors_error& e) {
  x_strings_exit(e.longMsg);
}

// Test if no error occurred:

bool x_errors_ok(x_errors_error& e) {
  return e.code == 0;
};

// Test if an error occurred:

bool x_errors_bool(x_errors_error& e) {
  return e.code != 0;
};

// Return the error code:

int32_t x_errors_code(x_errors_error& e) {
  return e.code;
};

// Retrieve the error message:

void x_errors_msg(x_errors_error& e, x_strings_string& res) {
  if (e.code == 0) {
    x_strings_init_empty(res);
  } else if (e.len == 0) { 
    x_strings_init_copy(res, e.longMsg);
  } else {
    x_strings_init_ptrSize(res, e.msg, e.len);
  }
};

// Error codes:

int32_t const x_errors_OUT__OF__BOUNDS = 1;
uint8_t const* const x_errors_OUT__OF__BOUNDS__MSG 
  = (uint8_t const*) "out of bounds";
int32_t const x_errors_ALLOCATION__FAILED = 2;
uint8_t const* const x_errors_ALLOCATION__FAILED__MSG
  = (uint8_t const*) "allocation failed";

}
