// translated from tokenizer.x - split a textual input into tokens

#include "tokenizer.h"

extern "C" {

// Constructor:

void x_tokenizer_init(x_tokenizer_tokenizer& t, uint8_t* s, size_t l) {

}

// Destructor:

void x_tokenizer_exit(x_tokenizer_tokenizer& t) {

}

// Check if there is more:

bool x_tokenizer_done(x_tokenizer_tokenizer& t) {
  return false;
}

// Analyize one token and advance:

x_tokenizer_TokenType x_tokenizer_next(x_tokenizer_tokenizer& t) {
  return x_tokenizer_TokenType::None;
}

// Skip or get whitespace:

void x_tokenizer_skipWhitespace(x_tokenizer_tokenizer& t) {

}

// Skip or get comment:

void x_tokenizer_skipComment(x_tokenizer_tokenizer& t) {

}

}   // extern "C"
