// translated from tokenizer.x - split a textual input into tokens

#include <string.h>
#include <math.h>

#include "tokenizer.h"

extern "C" {

char const* x_tokenizer_names[] = {
  "None ", "Error", "Open ", "Close", "SignI", "Unsig", "Float",
  "Strng", "Ident", "Oper " };

// Constructor:

void x_tokenizer_init(x_tokenizer_tokenizer& t, uint8_t* s, size_t l) {
  t.buf = s;
  t.len = l;
  t.pos = 0;
  t.row = 1;
  t.col = 1;
  t.typ = x_tokenizer_TokenType::None;
  t.i   = 0;
  t.u   = 0;
  t.f   = 0.0;
  x_strings_init_empty(t.s);
  t.c   = 0;
}

// Destructor:

void x_tokenizer_exit(x_tokenizer_tokenizer& t) {
  t.buf = nullptr;
  t.typ = x_tokenizer_TokenType::None;
  x_strings_exit(t.s);
}

// Check if there is more:

bool x_tokenizer_done(x_tokenizer_tokenizer& t) {
  return t.pos >= t.len;
}

// Skip whitespace:

static void x_tokenizer_skipWhitespace(x_tokenizer_tokenizer& t) {
  do {
    t.pos += 1;
  } while(t.pos < t.len && x_tokenizer_isWhitespace(t.buf[t.pos]));
}

// Skip comment:

static void x_tokenizer_skipComment(x_tokenizer_tokenizer& t) {
  size_t start = t.pos;
  do {
    t.pos += 1;
  } while (t.pos < t.len && t.buf[t.pos] != (uint8_t) '#');
  if (t.pos >= t.len) {
    return;
  }
  // We found the next #, was it ##?
  if (t.pos == start + 1) {
    // Skip till end of line:
    do {
      t.pos += 1;
    } while(t.pos < t.len && t.buf[t.pos] != '\n');
    if (t.pos < t.len) {
      t.pos += 1;
    }
    return;
  }
  t.pos += 1;
  size_t len = t.pos - start;
  while (t.pos < t.len) {
    if (t.buf[t.pos] == (uint8_t) '#') {
      if (t.pos + len >= t.len) {
        t.pos = t.len;
        return;
      }
      if (memcmp(t.buf + t.pos, t.buf + start, len) == 0) {
        t.pos += len;
        return;
      }
    }
    t.pos += 1;
  }
}

// Skip one UTF8 code point.

static void x_tokenizer_skipUTF8CodePoint(x_tokenizer_tokenizer& t) {
  do {
    t.pos += 1;
  } while (t.pos < t.len && t.buf[t.pos] >= (uint8_t) 0x80);
}

// Scan an integer with a given base, stop at any character that is not
// a digit of that base

static uint8_t valTab[256] = {
  16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,   // 0-15
  16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,   // 16-31
  16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,   // 32-47
   0, 1, 2, 3, 4, 5, 6, 7, 8, 9,16,16,16,16,16,16,   // 48-63
  16,10,11,12,13,14,15,16,16,16,16,16,16,16,16,16,   // 64-79
  16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,17,   // 80-95
  16,10,11,12,13,14,15,16,16,16,16,16,16,16,16,16,   // 64-111
  16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,   // 112-127
  16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
  16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
  16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
  16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
  16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
  16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
  16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
  16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16};

static uint64_t x_tokenizer_limits[17] =
  { 0, 0, 0x8000000000000000ull, 0, 0, 0, 0, 0, 0x2000000000000000ull, 0,
    0x199999999999999Aull, 0, 0, 0, 0, 0, 0x1000000000000000ull };

static uint64_t x_tokenizer_scanUInt64(x_tokenizer_tokenizer& t,
                                       uint64_t base, bool& overflow) {
  uint64_t res = 0;
  uint64_t limit = x_tokenizer_limits[base];
  overflow = false;
  do {
    uint8_t v = valTab[t.buf[t.pos]];
    if (v != 17) {  // underscore ignored
      if (v >= base) {   // includes 16
        break;
      }
      if (res >= limit) {  // check for overflow
        overflow = true;
      }
      res = res * base + v;
      if (res < v) {
        overflow = true;
      }
    }
    t.pos += 1;
  } while (t.pos < t.len);
  return overflow ? 0 : res;
}

// Determine the base, this is called when we are standing on the first
// character of a number (after + or -, if it starts with that):

uint64_t x_tokenizer_findBase(x_tokenizer_tokenizer& t) {
  uint64_t base = 10;
  if (t.buf[t.pos] == (uint8_t) '0' && t.pos + 1 < t.len) {
    if (t.buf[t.pos + 1] == (uint8_t) 'x') {
      base = 16;
      t.pos += 2;
    } else if (t.buf[t.pos + 1] == (uint8_t) 'o') {
      base = 8;
      t.pos += 2;
    } else if (t.buf[t.pos + 1] == (uint8_t) 'b') {
      base = 2;
      t.pos += 2;
    }
  }
  return base;
}

// Scan a float, this is called when we have scanned the integral part
// and have just found the dot:

static x_tokenizer_TokenType x_tokenizer_scanFloat(x_tokenizer_tokenizer& t,
                                                   int64_t intPart,
                                                   uint64_t base,
                                                   bool overflow,
                                                   bool minus) {
  double dbase = (double) base;
  double po = 1.0 / dbase;
  double v = (double) intPart;
  t.pos += 1;
  while (t.pos < t.len) {
    uint8_t w = valTab[t.buf[t.pos]];
    if (w != 17) {   // underscore ignored
      if (w >= base) {   // includes 16
        break;
      }
      v += po * (double) w;
      po /= dbase;
    }
    t.pos += 1;
  }
  // Now finally check for an exponent:
  if (t.pos < t.len && t.buf[t.pos] == (uint8_t) '*') {
    t.pos += 1;
    if (t.pos + 1 < t.len &&
        t.buf[t.pos] == (uint8_t) '2' &&
        t.buf[t.pos + 1] == (uint8_t) '^') {
      dbase = 2.0;
      t.pos += 2;
    } else if (t.pos + 1 < t.len &&
               t.buf[t.pos] == (uint8_t) '8' &&
               t.buf[t.pos + 1] == (uint8_t) '^') {
      dbase = 8.0;
      t.pos += 2;
    } else if (t.pos + 1 < t.len &&
               t.buf[t.pos] == (uint8_t) 'x' &&
               t.buf[t.pos + 1] == (uint8_t) '^') {
      dbase = 16.0;
      t.pos += 2;
    } else if (t.pos + 2 < t.len &&
               t.buf[t.pos] == (uint8_t) '1' &&
               t.buf[t.pos + 1] == (uint8_t) '0' &&
               t.buf[t.pos + 2] == (uint8_t) '^') {
      dbase = 10.0;
      t.pos += 3;
    } else {
      t.typ = x_tokenizer_TokenType::Error;
      return t.typ;
    }
    // Now read the exponent:
    if (t.pos < t.len) {
      bool expMinus = false;
      if (t.buf[t.pos] == (uint8_t) '-') {
        t.pos += 1;
        expMinus = true;
      }
      if (t.pos < t.len) {
        bool expOverflow;
        uint64_t e = x_tokenizer_scanUInt64(t, base, expOverflow);
        if (expOverflow || e >= 1024) {
          t.typ = x_tokenizer_TokenType::Error;
          return t.typ;
        }
        v *= pow(dbase, expMinus ? -(double)e : (double) e);
      }
    }
  }
  if (overflow) {
    t.typ = x_tokenizer_TokenType::Error;
    return t.typ;
  }
  t.typ = x_tokenizer_TokenType::Float;
  t.f = minus ? -v : v;
  return t.typ;
}

// Scan a signed number (or float) , only called when standing on '-' or
// '+' and a digit is following:

static x_tokenizer_TokenType x_tokenizer_scanSigned(x_tokenizer_tokenizer& t) {
  bool minus = t.buf[t.pos] == (uint8_t) '-';
  t.pos += 1;
  uint64_t base = x_tokenizer_findBase(t);
  bool overflow = false;
  uint64_t v = x_tokenizer_scanUInt64(t, base, overflow);
  if (t.pos >= t.len || t.buf[t.pos] != (uint8_t) '.') {
    if (overflow || (minus && v >= 0x8000000000000000ull)) {
      t.typ = x_tokenizer_TokenType::Error;
    } else {
      t.i = minus ? -(int64_t) v : (int64_t) v;;
      t.typ = x_tokenizer_TokenType::SignedInt;
    }
    return t.typ;
  }
  return x_tokenizer_scanFloat(t, v, base, overflow, minus);
}

// Scan an unsigned number (or float), only called when standing on a digit:

static x_tokenizer_TokenType x_tokenizer_scanUnsigned(x_tokenizer_tokenizer& t) {
  uint64_t base = x_tokenizer_findBase(t);
  bool overflow = false;
  uint64_t v = x_tokenizer_scanUInt64(t, base, overflow);
  if (t.pos >= t.len || t.buf[t.pos] != (uint8_t) '.') {
    t.u = v;
    t.typ = overflow ? x_tokenizer_TokenType::Error
                     : x_tokenizer_TokenType::UnsignedInt;
    return t.typ;
  }
  return x_tokenizer_scanFloat(t, v, base, overflow, false);
}

// Scan an operator called when standing on an operator symbol:

static x_tokenizer_TokenType x_tokenizer_scanOperator(x_tokenizer_tokenizer& t) {
  size_t start = t.pos;
  do {
    t.pos += 1;
  } while (t.pos < t.len && x_tokenizer_isOp(t.buf[t.pos]));
  x_strings_set_ptrSize(t.s, t.buf + start, t.pos - start);
  t.typ = x_tokenizer_TokenType::Operator;
  return t.typ;
}

// Recall UTF-8:
// 0xxxxxxx
// 110xxxxx 10xxxxxx
// 1110xxxx 10xxxxxx 10xxxxxx
// 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
// Helper to read a code point in UTF-8:

static uint32_t x_tokenizer_readCodepoint(uint8_t* p) {
  uint8_t c = *p;
  if (c < 0b10000000) {
    return (uint32_t) c;
  } else if (c < 0b11100000) {
    return ((uint32_t) (c & 0b00011111) << 6) |
           ((uint32_t) (p[1] & 0b00111111));
  } else if (c < 0b11110000) {
    return ((uint32_t) (c & 0b00001111) << 12) |
           ((uint32_t) (p[1] & 0b00111111) << 6) |
           ((uint32_t) (p[2] & 0b00111111));
  } else {
    return ((uint32_t) (c & 0b00000111) << 18) |
           ((uint32_t) (p[1] & 0b00111111) << 12) |
           ((uint32_t) (p[2] & 0b00111111) << 7) |
           ((uint32_t) (p[3] & 0b00111111));
  }
}

// Helper to write out a code point:

static void x_tokenizer_append_codepoint(x_strings_string& s, uint32_t cp) {
  if (cp < 0x80u) {
    s.buf[s.size] = (uint8_t) cp;
    s.size += 1;
    return;
  }
  if (cp < 0x800u) {
    s.buf[s.size] = (uint8_t) ((cp >> 6) | 0xc0u);
    s.buf[s.size+1] = (uint8_t) ((cp & 0x3fu) | 0x80u);
    s.size += 2;
    return;
  }
  if (cp < 0x10000u) {
    s.buf[s.size] = (uint8_t) ((cp >> 12) | 0xe0u);
    s.buf[s.size+1] = (uint8_t) (((cp >> 6) & 0x3fu) | 0x80u);
    s.buf[s.size+2] = (uint8_t) ((cp & 0x3fu) | 0x80u);
    s.size += 3;
    return;
  }
  if (cp < 0x200000u) {
    s.buf[s.size] = (uint8_t) ((cp >> 18) | 0xf0u);
    s.buf[s.size+1] = (uint8_t) (((cp >> 12) & 0x3fu) | 0x80u);
    s.buf[s.size+2] = (uint8_t) (((cp >> 6) & 0x3fu) | 0x80u);
    s.buf[s.size+3] = (uint8_t) ((cp & 0x3fu) | 0x80u);
    s.size += 4;
    return;
  }
}

// Helper to read a hex number with a given number of digits:

static uint64_t x_tokenizer_readHex(uint8_t* p, size_t len) {
  uint64_t res = 0;
  for (size_t i = 0; i < len; ++i) {
    uint8_t v = valTab[p[i]];
    if (v < 16) {
      res = (res << 4) + v;
    }
  }
  return res;
}

// Helper routine for string scanning, called when standing directly behind
// the opening quotes character:

static void x_tokenizer_findStringEnd(x_tokenizer_tokenizer& t,
                                      bool& error,
                                      bool& isChar) {
  error = false;
  isChar = false;
  while (t.pos < t.len) {
    char c = (char) t.buf[t.pos];
    if (c == '"') {
      t.pos += 1;
      return;
    }
    if (c == '\n' || c == '\r') {
      error = true;
      return;
    }
    if (c == '\\') {
      t.pos += 1;
      if (t.pos >= t.len) {
        error = true;
        return;
      }
      c = (char) t.buf[t.pos];
      if (c == 't' || c == '\\' || c == 'n' || c == 'r' || c == '"') {
        t.pos += 1;
      } else if (c == 'c') {
        t.pos += 1;
        isChar = true;
      } else if (c == 'x') {
        t.pos += 1;
        if (t.pos + 2 > t.len) {
          error = true;
          return;
        }
        t.pos += 2;
      } else if (c == 'u') {
        t.pos += 1;
        if (t.pos + 4 > t.len) {
          error = true;
          return;
        }
        t.pos += 4;
      } else if (c == 'U') {
        t.pos += 1;
        if (t.pos + 8 > t.len) {
          error = true;
          return;
        }
        t.pos += 8;
      } if (c == '|') {
        size_t rawStart = t.pos;
        do {
          t.pos += 1;
        } while (t.pos < t.len && t.buf[t.pos] != (uint8_t) '|');
        if (t.pos >= t.len) {
          error = true;
          return;
        }
        // We found the next |
        t.pos += 1;
        size_t rawLen = t.pos - rawStart;
        error = true;
        while (t.pos < t.len) {
          if (t.buf[t.pos] != (uint8_t) '|') {
            if (t.pos + rawLen >= t.len) {
              t.pos = t.len;
              return;
            }
            if (memcmp(t.buf + t.pos, t.buf + rawStart, rawLen) == 0) {
              t.pos += rawLen;
              error = false;
              break;
            }
          }
          t.pos += 1;
        }
      }
      // If we fall through here, we simply take the backslash
    } else {
      t.pos += 1;
    }
  }
}

// Helper routine for string scanning, called when standing directly behind
// the opening quotes character:

static void x_tokenizer_copyString(x_tokenizer_tokenizer& t, size_t pos) {
  while (pos < t.len) {
    char c = (char) t.buf[pos];
    if (c == '"') {
      return;
    }
    if (c == '\\') {
      pos += 1;
      c = (char) t.buf[pos];
      if (c == 't') {
        x_strings_append_char(t.s, (uint8_t) '\t');
        pos += 1;
      } else if (c == '\\') {
        x_strings_append_char(t.s, (uint8_t) '\\');
        pos += 1;
      } else if (c == 'n') {
        x_strings_append_char(t.s, (uint8_t) '\n');
        pos += 1;
      } else if (c == 'r') {
        x_strings_append_char(t.s, (uint8_t) '\n');
        pos += 1;
      } else if (c == '"') {
        x_strings_append_char(t.s, (uint8_t) '"');
        pos += 1;
      } else if (c == 'c') {
        pos += 1;
      } else if (c == 'x') {
        pos += 1;
        uint8_t x = (uint8_t) x_tokenizer_readHex(t.buf + pos, 2);
        x_strings_append_char(t.s, x);
        pos += 2;
      } else if (c == 'u') {
        pos += 1;
        uint32_t x = (uint32_t) x_tokenizer_readHex(t.buf + pos, 4);
        x_tokenizer_append_codepoint(t.s, x);
        pos += 4;
      } else if (c == 'U') {
        pos += 1;
        uint64_t x = x_tokenizer_readHex(t.buf + pos, 8);
        x_tokenizer_append_codepoint(t.s, x);
        pos += 8;
      } if (c == '|') {
        size_t rawStart = pos;
        do {
          pos += 1;
        } while (pos < t.len && t.buf[pos] != (uint8_t) '|');
        // We found the next |
        pos += 1;
        size_t rawLen = pos - rawStart;
        while (pos < t.len) {
          if (t.buf[pos] != (uint8_t) '|') {
            if (pos + rawLen >= t.len) {
              pos = t.len;
              return;
            }
            if (memcmp(t.buf + pos, t.buf + rawStart, rawLen) == 0) {
              pos += rawLen;
              x_strings_append_chars(t.s, t.buf + rawStart + rawLen,
                                          pos - (rawStart + rawLen));
              break;
            }
          }
          pos += 1;
        }
      }
      // If we fall through here, we simply take the backslash
    } else {
      x_strings_append_char(t.s, t.buf[pos]);
      pos += 1;
    }
  }
}

// Scan a string called when standing on a " character:

static x_tokenizer_TokenType x_tokenizer_scanString(x_tokenizer_tokenizer& t) {
  t.pos += 1;
  size_t start = t.pos;
  bool error;
  bool isChar;
  x_tokenizer_findStringEnd(t, error, isChar);
  if (error) {
    t.typ = x_tokenizer_TokenType::Error;
    return t.typ;
  }
  // Now we have found the end of the string, thus we have an upper bound
  // for the size:
  x_strings_reserve(t.s, t.pos - start);
  t.s.size = 0;
  x_tokenizer_copyString(t, start);
  if (isChar) {
    t.u = x_tokenizer_readCodepoint(x_strings_c__str(t.s));
    t.typ = x_tokenizer_TokenType::UnsignedInt;
  } else {
    t.typ = x_tokenizer_TokenType::String;
  }
  return t.typ;
}

// Scan an identifier called when standing on a letter, underscore or /
// with a letter or underscore folloing:

static x_tokenizer_TokenType x_tokenizer_scanIdentifier(x_tokenizer_tokenizer& t) {
  size_t start = t.pos;
  do {
    t.pos += 1;
  } while (t.pos < t.len && (x_tokenizer_isLetter(t.buf[t.pos]) ||
                             t.buf[t.pos] == (uint8_t) '/'));
  x_strings_set_ptrSize(t.s, t.buf + start, t.pos - start);
  t.typ = x_tokenizer_TokenType::Identifier;
  return t.typ;
}

// Analyize one token and advance:

x_tokenizer_TokenType x_tokenizer_next(x_tokenizer_tokenizer& t) {
  while (true) {
    if (t.pos >= t.len) {
      t.typ = x_tokenizer_TokenType::None;
      return t.typ;
    } 
    uint8_t c = t.buf[t.pos];
    switch ((char) c) {
      case '+':
      case '-':
        if (t.pos + 1 < t.len && x_tokenizer_isDigit(t.buf[t.pos+1])) {
          return x_tokenizer_scanSigned(t);
        }
        return x_tokenizer_scanOperator(t);
      case '/':
        if (t.pos + 1 < t.len && x_tokenizer_isLetter(t.buf[t.pos+1])) {
          return x_tokenizer_scanIdentifier(t);
        }
        return x_tokenizer_scanOperator(t);
      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
        return x_tokenizer_scanUnsigned(t);
      case '(': case '[': case '{':
        t.c = c;
        t.typ = x_tokenizer_TokenType::Open;
        t.pos += 1;
        return t.typ;
      case ')': case ']': case '}':
        t.c = c;
        t.typ = x_tokenizer_TokenType::Close;
        t.pos += 1;
        return t.typ;
      case '"':
        return x_tokenizer_scanString(t);
      case ' ': case '\t': case '\n': case '\r':
        x_tokenizer_skipWhitespace(t);
        break;
      case '#':
        x_tokenizer_skipComment(t);
        break;
      default:
        if (x_tokenizer_isLetter(c)) {
          return x_tokenizer_scanIdentifier(t);
        } else if (x_tokenizer_isOp(c)) {
          return x_tokenizer_scanOperator(t);
        } else {
          t.typ = x_tokenizer_TokenType::Error;
          x_tokenizer_skipUTF8CodePoint(t);
          return t.typ;
        }
    }
  }
  return x_tokenizer_TokenType::None;
}

}   // extern "C"
