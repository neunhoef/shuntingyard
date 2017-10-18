#---# tokenizer.x - split a textual input into tokens

This module contains the X tokenizer.
---

namespace := /x/tokenizer;
search := [/x/];   ## this is the default and could be omitted

## Token types:

TokenType := type : enum[uint8]{
  None := 0;
  Comment := 1;
  Whitespace := 2;
  Error := 3;
  ## From here on proper types which produce tokens:
  Open := 4;
  Close := 5;
  Number := 6;
  String := 7;
  Identifier := 8;
  Operator := 9;
};
  
## The data type of a tokenizer:

tokenizer ::= type : struct[
  buf : ptr[uint8];
  len : uint;
  pos : ptr[uint8];
  row : uint;
  col : uint;
];

## Constructor:

init := func(t --> tokenizer, s <- ptr[uint8], l <- uint) {
  t.buf := s;
  t.len := l;
  t.pos := 0;
  t.row := 1;
  t.col := 1;
};

## Destructor:

exit := func(t <--> tokenizer) {
  t.buf := ptr[uint8]{0u};
};

## Check if there is more:

done := func(t <-- tokenizer, r -> bool) {
  r := t.pos < t.len;
};

## Determine type of next token:

nextType := func(t <-- tokenizer, r -> TokenType) {
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

## Get current character (used for TokenType/Open and TokenType/Close:

getChar ::= func(t <--> tokenizer, r -> uint8) {
  r := t.buf[t.pos];
  t.pos := t.pos + 1;
}

## Skip or get whitespace.

skipWhitespace ::= func(t <--> tokenizer) {
  ...
}

getWhitespace ::= func(t <--> tokenizer, r --> string) {
  ...
}

## Skip or get comment:

skipComment ::= func(t <--> tokenizer) {
  ...
}

getComment ::= func(t <--> tokenizer, r --> string) {
  ...
}

## Get a number:

NumberResult ::= type : struct[
  u : uint64;
 offset(0);
  i : int64;
 offset(0);
  f : float64;
  type: uint8;   ## 0 : uint64, 1 : int64, 2 : float64
];

getNumber ::= func(t <--> tokenizer, r --> NumberResult) {
  ...
}

## Get a string:

getString ::= func(t <--> tokenizer, r --> string) {
  ...
}

## Get an identifier:

getIdentifier ::= func(t <--> tokenizer, p -> ptr[uint8], l -> uint) {
  ...
}

## Get an operator:

getOperator ::= func(t <--> tokenizer, p --> ptr[uint8], l -> uint) {
  ...
}

