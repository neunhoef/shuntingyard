#---# error.x - error handling

This module contains the fundamental definitions for the error handling
in X. The rest of the support is in interpreter and compiler in the form
of a thread local, static and global variable `error` of type `error`
---

namespace := /x/errors;
search := [/x/];   ## this is the default and could be omitted

## The data type itself:

error ::= type : struct[
  code : int32;
  len : uint32;
  msg : array[96][uint8];
  longMsg : string;
  data : ptr[niltype];
];

## Constructors:

init/empty ::= func(e --> error) {
  init(e.longMsg);
  set(e);
}

init/msg ::= func(e --> error, c <- int32, m <- ptr[uint8]) {
  init(e.longMsg);
  set(e, c, m);
}

## Setters:

set/empty ::= func(e --> error) {
  e.code := int32{0};
  e.len := int32{0};
  set(e.longMsg);
  e.data := ptr[niltype]{0u};
}

set/msg ::= func(e --> error, c <- int32, m <- ptr[uint8]) {
  e.code := c;
  size := var : uint{/C/string/strlen(m)};
  if (size <= 96) {
    e.len = uint32{size};
    /C/string/memcpy(e.msg, m, size);
    set(e.longMsg);
  } {  ## else
    e.len = 0;
    set(e.longMsg, m, size);
  }
  e.data := ptr[niltype]{0u};
}

## Destructor:

exit ::= func(e <--> error) {
  exit(e.longMsg);
}

## Test if no error occurred:

ok ::= func(e <-- error, res -> bool) {
  res := e.code = 0;
};

## Test if an error occurred:

bool ::= func(e <-- error, res -> bool) {
  res := e.code != 0;
};

## Return the error code:

code ::= func(e <-- error, res -> int32) {
  res := e.code;
};

## Retrieve the error message:

msg ::= func(e <-- error, res -> string) {
  if (e.code = 0) {
    init(res);
  } (e.len = 0) { 
    res := e.longMsg;
  } {
    init(res, e.msg, e.len);
  }
};

## Error codes:

OUT_OF_BOUNDS ::= const : int32{1};
OUT_OF_BOUNDS_MSG ::= const : ptr[uint8]{"out of bounds"};
ALLOCATION_FAILED ::= const : int32{2};
ALLOCATION_FAILED_MSG ::= const : ptr[uint8]{"allocation failed"};

