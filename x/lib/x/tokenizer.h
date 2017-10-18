// translated from tokenizer.x - split a textual input into tokens

#ifndef X_TOKENIZER_H
#define X_TOKENIZER_H 1

#include <cstdint>

#include "errors.h"
#include "strings.h"

extern "C" {

// Token types:

enum class x_tokenizer_TokenType : uint8_t {
  None = 0,
  Comment = 1,
  Whitespace = 2,
  Error = 3,
  Open = 4,
  Close = 5,
  SignedInt = 6,
  UnsignedInt = 7,
  Float = 8,
  String = 9,
  Identifier = 10,
  Operator = 11
};
  
// The data type of a tokenizer:

struct x_tokenizer_tokenizer {
  uint8_t* buf;
  size_t   len;
  uint8_t* pos;
  size_t   row;
  size_t   col;
  int64_t  i;   // valid if and only if typ = SignedInt
  uint64_t u;   // valid if and only if typ = UnsignedInt
  double   f;   // valid if and only if typ = Float
  x_strings_string s;  // valid if and only if typ in 
                       // { String, Identifier, Operator }
  uint8_t  c;   // valid if and only if typ in { Open, Close }
  x_tokenizer_TokenType typ;
};

// Constructor:

void x_tokenizer_init(x_tokenizer_tokenizer& t, uint8_t* s, size_t l);

// Destructor:

void x_tokenizer_exit(x_tokenizer_tokenizer& t);

// Check if there is more:

bool x_tokenizer_done(x_tokenizer_tokenizer& t);

// Analyize one token and advance:

x_tokenizer_TokenType x_tokenizer_next(x_tokenizer_tokenizer& t);

// Getter current character (used for TokenType/Open and TokenType/Close:

static inline x_tokenizer_TokenType x_tokenizer_type(x_tokenizer_tokenizer& t) {
  return t.typ;
}

static inline uint8_t x_tokenizer_get_char(x_tokenizer_tokenizer& t) {
  return t.c;
}

static inline int64_t x_tokenizer_get_int(x_tokenizer_tokenizer& t) {
  return  t.i;
}

static inline uint64_t x_tokenizer_get_uint(x_tokenizer_tokenizer& t) {
  return t.u;
}

static inline double x_tokenizer_get_float(x_tokenizer_tokenizer& t) {
  return t.f;
}

static inline void x_tokenizer_get_string(x_tokenizer_tokenizer& t,
                                          x_strings_string& r) {
  x_strings_init_copy(r, t.s);
};

// Skip or get whitespace:

void x_tokenizer_skipWhitespace(x_tokenizer_tokenizer& t);

// Skip or get comment:

void x_tokenizer_skipComment(x_tokenizer_tokenizer& t);

}

#endif

