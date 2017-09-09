## bindings to C for string.h

namespace := /C/string;;

strlen := func(p <- ptr[uint8], len -> uint) [
  extern("strlen"), lib("C")
];

