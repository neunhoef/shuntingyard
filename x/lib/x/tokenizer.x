#---# tokenizer.x - split a textual input into tokens

This module contains the X tokenizer.
#---#

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
  
## The data type of a token:

token ::= type : struct[
  offset(0)[
    i :: int64;   ## valid if and only if typ = SignedInt
    u :: uint64;  ## valid if and only if typ = UnsignedInt
    f :: float64; ## valid if and only if typ = Float
    c :: uint8;   ## valid if and only if typ in the set { Open, Close }
    s :: string;  ## valid if and only if typ in the set
                  ## { String, Identifier, Operator }
  ];
  pos :: uint;    ## position in the tokenized text
  typ :: TokenType;
];

## The data type of a tokenizer:

tokenizer ::= type : struct[
  buf : ptr[uint8];
  len : uint;
  pos : uint;
  tok :: token;
];

## Constructors:

init/token ::= func(t --> token) {
  t.i := 0;
  t.pos := 0;
  t.typ := TokenType/None;
}

init/tokenizer ::= func(t --> tokenizer, s <- ptr[uint8], l <- uint) {
  t.buf := s;
  t.len := l;
  t.pos := 0;
  init(t.tok);
};

## Destructors:

exit/token ::= func(t <--> token) {
  if (t.typ = TokenType/String ||
      t.typ = TokenType/Identifier ||
      t.typ = TokenType/Operator) {
    exit(t.s);
  }
  t.typ := TokenType/None;
}

exit/tokenizer ::= func(t <--> tokenizer) {
  t.buf := ptr[uint8]{0};
  exit(t.tok);
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
  if (c >= "\c0" && c <= "\c9") {
    r := TokenType/Number;
  } (c = "\c\"") {
    r := TokenType/String;
  } (c = "\c(" || c = "\c[" || c = "\c{") {
    r := TokenType/Open;
  } (c = "\c)" || c = "\c]" || c = "\c}") {
    r := TokenType/Close;
  } (c = "\c#") {
    r := TokenType/Comment;
  } (isOp(c)) {
    r := TokenType/Operator;
  } (isWhite(c)) {
    r := TokenType/Whitespace;
  } () {
    ...;
  }
}

## Skip or get whitespace:

skipWhitespace := func(t <--> tokenizer) {
  ...
}

## Skip or get comment:

skipComment ::= func(t <--> tokenizer) {
  ...
}

## Setters:

set/int ::= func(t <--> token, i <- int64) {
  if (t.typ = TokenType ...
  t.typ = TokenType/SignedInt;
  t.i = i;
