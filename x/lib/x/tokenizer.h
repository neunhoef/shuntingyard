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
  Error = 1,
  Open = 2,
  Close = 3,
  SignedInt = 4,
  UnsignedInt = 5,
  Float = 6,
  String = 7,
  Identifier = 8,
  Operator = 9
};

extern char const* x_tokenizer_names[];

static inline char const* x_tokenizer_name(x_tokenizer_TokenType tt) {
  return x_tokenizer_names[(uint8_t) tt];
}

// The data type of a tokenizer:

struct x_tokenizer_token {
  union {
    int64_t i;   // valid if and only if typ = SignedInt
    uint64_t u;  // valid if and only if typ = UnsignedInt
    double f;    // valid if and only if typ = Float
    uint8_t c;   // valid if and only if typ in { Open, Close }
    x_strings_string s;  // valid if and only if typ in 
                         // { String, Identifier, Operator }
  };
  size_t pos;    // byte position in tokenized text
  x_tokenizer_TokenType typ;
};

struct x_tokenizer_tokenizer {
  uint8_t* buf;
  size_t   len;
  size_t   pos;
  x_tokenizer_token tok;
};

// Constructors:

void x_tokenizer_init_token(x_tokenizer_token& t);

void x_tokenizer_init_tokenizer(x_tokenizer_tokenizer& t, uint8_t* s, size_t l);

// Destructors:

void x_tokenizer_exit_token(x_tokenizer_token& t);

void x_tokenizer_exit_tokenizer(x_tokenizer_tokenizer& t);

// Check if there is more:

bool x_tokenizer_done(x_tokenizer_tokenizer& t);

// Analyize one token and advance:

x_tokenizer_TokenType x_tokenizer_next(x_tokenizer_tokenizer& t);

static inline bool x_tokenizer_isOp(uint8_t cc) {
  char c = (char) cc;
  return c == '!' || c == '$' || c == '%' || c == '&' || c == '\'' ||
         c == '*' || c == '+' || c == ',' || c == '-' || c == '.' ||
         c == '/' || c == ':' || c == ';' || c == '<' || c == '=' ||
         c == '>' || c == '?' || c == '@' || c == '\\' || c == '^' ||
         c == '`' || c == '|' || c == '~';
}

static inline bool x_tokenizer_isLetter(uint8_t c) {
  return (c >= (uint8_t) 'a' && c <= (uint8_t) 'z') ||
         (c >= (uint8_t) 'A' && c <= (uint8_t) 'Z') || c == (uint8_t) '_';
}

static inline bool x_tokenizer_isDigit(uint8_t c) {
  return c >= (uint8_t) '0' && c <= (uint8_t) '9';
}

static inline bool x_tokenizer_isWhitespace(uint8_t cc) {
  char c = (char) cc;
  return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

}

#endif

