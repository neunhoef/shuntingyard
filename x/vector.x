## Low level routines for vectors

namespace := /x/internal/vector;
search := [ /x/internal/types ];

vector := type( 
  struct{
    objSize   : uint,
    allocSize : uint,
    usedSize  : uint,
    buf       : ptr(uint8)
  }
);

init := func(v <- ptr(vector), objSize <- uint, initialSize <- uint);
exit := func(v <- ptr(vector));
size := func(v <- ptr(vector), result -> uint);
alloc_size := func(v <- ptr(vector), result -> uint);
reserve := func(v <- ptr(vector), size <- uint, result -> intc);
shrink := func(v <- ptr(vector));
push_back := func(v <- ptr(vector), data <- ptr(uint8));
pop_back := func(v <- ptr(vector), result <- ptr(uint8));
front := func(v <- ptr(vector), result <- ptr(uint8));
back := func(v <- ptr(vector), result <- ptr(uint8));
at := func(v <- ptr(vector), pos <- uint, result <- ptr(uint8));
assign := func(v <- ptr(vector), pos <- uint, val <- ptr(uint8));

