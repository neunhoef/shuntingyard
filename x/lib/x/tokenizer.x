#---# tokenizer.x - split a textual input into tokens

This module contains the X tokenizer.
---

namespace := /x/tokenizer;
search := [/x/errors, /x/strings];

## Token types:

TokenType := type : enum[uint8]{
  None := 0;
  Comment := 1;
  Whitespace := 2;
  Error := 3;
  ## From here on proper types which produce tokens:
  Open := 4;
  Close := 5;
  SignedInt := 6;
  UnsignedInt := 7;
  Float := 8;
  String := 9;
  Identifier := 10;
  Operator := 11;
};
  
## The data type of a tokenizer:

tokenizer ::= type : struct[
  buf : ptr[uint8];
  len : uint;
  pos : ptr[uint8];
  row : uint;
  col : uint;
  i   : int64;   ## valid if and only if typ = SignedInt
  u   : uint64;  ## valid if and only if typ = UnsignedInt
  f   : float64; ## valid if and only if typ = Float
  s   : string;  ## valid if and only if typ in { String, Identifier, Operator }
  c   : uint8;   ## valid if and only if typ in { Open, Close }
  typ : TokenType;
];

## Constructor:

init ::= func(t --> tokenizer, s <- ptr[uint8], l <- uint) {
  t.buf := s;
  t.len := l;
  t.pos := 0;
  t.row := 1;
  t.col := 1;
  t.typ := TokenType/None;
  t.i   := +0;
  t.u   := 0;
  t.f   := 0.0;
  init(t.s);
  t.c   := uint8{0};
};

## Destructor:

exit ::= func(t <--> tokenizer) {
  t.buf := ptr[uint8]{0};
  t.typ := TokenType/None;
  exit(s);
};

## Check if there is more:

done ::= func(t <-- tokenizer, r -> bool) {
  r := t.pos >= t.len;
};

## Analyize one token and advance:

next ::= func(t <-- tokenizer, r -> TokenType) {
  if (t.pos >= t.len) {
    r := TokenType/None;
    return;
  } 
  c := var : uint32{t.buf[t.pos]};
  if (c >= '0' && c <= '9') {
    r := TokenType/Number;
  } (c = '"') {
    r := TokenType/String;
  } (c = '(' || c = '[' || c = '{') {
    r := TokenType/Open;
  } (c = ')' || c = ']' || c = '}') {
    r := TokenType/Close;
  } (c = '#') {
    r := TokenType/Comment;
  } (isOp(c)) {
    r := TokenType/Operator;
  } (isWhite(c)) {
    r := TokenType/Whitespace;
  } () {
    ...;
  }
}

## Getter current character (used for TokenType/Open and TokenType/Close:

type := func(t <-- tokenizer, r -> TokenType) {
  r := t.typ;
};

get/char ::= func(t <--> tokenizer, r -> uint8) {
  r := t.c;
};

get/int ::= func(t <--> tokenizer, r -> int) {
  r := t.i;
};

get/uint ::= func(t <--> tokenizer, r -> uint) {
  r := t.u;
};

get/float ::= func(t <--> tokenizer, r -> float64) {
  r := t.f;
};

get/string ::= func(t <--> tokenizer, r -> string) {
  r := t.s;
};

## Skip or get whitespace:

skipWhitespace := func(t <--> tokenizer) {
  ...
}

## Skip or get comment:

skipComment ::= func(t <--> tokenizer) {
  ...
}

