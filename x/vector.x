## Low level routines for vectors

namespace := /x/internal/vector;
search := [ /x/internal/types ];

vector := type( 
  struct{
    objSize   : uint,
    allocSize : uint,
    usedSize  : uint,
    buf       : ptr ^ uint8
  }
);

init := func(v <- vector, objSize <- uint, initialSize <- uint);
exit := func(v <- vector);
size := func(v <- vector, result -> uint);
alloc_size := func(v <- vector, result -> uint);
reserve := func(v <- vector, size <- uint, result -> intc);
shrink := func(v <- vector);
push_back := func(v <- vector, data <- ptr ^ uint8);
pop_back := func(v <- vector, result <- ptr ^ uint8);
front := func(v <- vector, result <- ptr ^ uint8);
back := func(v <- vector, result <- ptr ^ uint8);
at := func(v <- vector, pos <- uint, result <- ptr ^ uint8);
assign := func(v <- vector, pos <- uint, val <- ptr ^ uint8);

