## Low level routines for vectors

namespace := /x/internal/vector;
search := [ /x/internal/types ];

vector := type( 
  struct[
    oSize : uint,
    alloc : uint,
    used  : uint,
    buf   : ptr[uint8]
  ]
);

## Constructor for a vector:

construct := func(v <- vector, objSize <- uint, initialAlloc <- uint) {
  v.oSize := objSize;
  v.alloc := initialAlloc;
  v.used := 0u;
  if (initialAlloc > 0u) {
    v.buf := /C/stdlib/malloc(objSize * initialAlloc);
    if (isNull(v.buf)) {
      v.alloc := 0;
    }
  } {  ## else
    v.buf := ptr[uint8]{0};
  }
}

construct := func(v <- vector, objSize <- uint) {
  init(v, objSize, 0);
}

destruct := func(v <- vector) {
  if (not(isNull(v.buf))) {
    /C/stdlib/free(v.buf);
    v.buf := ptr[uint8]{0};
  }
  v.alloc := 0;
  v.used := 0;
}

size := func(v <- vector, result -> uint) {
  result := v.used;
}

allocSize := func(v <- vector, result -> uint) {
  result := v.alloc;
}

reserve := func(v <- vector, size <- uint, result -> int) {
  
}

shrink := func(v <- vector);
push_back := func(v <- vector, data <- ptr[uint8]);
pop_back := func(v <- vector, result <- ptr[uint8]);
front := func(v <- vector, result <- ptr[uint8]);
back := func(v <- vector, result <- ptr[uint8]);
at := func(v <- vector, pos <- uint, result <- ptr[uint8]);
assign := func(v <- vector, pos <- uint, val <- ptr[uint8]);

