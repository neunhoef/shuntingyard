## bindings to C for stdlib.h

namespace := /C/stdlib;

malloc := func(size <- uint, res -> ptr[nil]) [
  extern("malloc"), lib("C")
];

free := func(p <- ptr[nil]) [
  extern("free"), lib("C")
];

calloc := func(nmemb <- uint, size <- uint, res -> ptr[nil]) [
  extern("calloc"), lib("C")
];

realloc := func(p <- ptr[nil], size <- uint, res -> ptr[nil]) [
  extern("realloc"), lib("C")
];


