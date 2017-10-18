#---# string.x - Strings
This library implements strings as UTF-8 byte sequences. It underpins
the builtin type /x/string.
---

namespace := /x;
search := [/x/];   ## This is the default and this could be omitted.

string ::= type :
  struct[
    buf   : ptr[uint8];
    alloc : uint;
    size  : uint;
  ]
);

## Constructors:

init/empty ::= func(s --> string) {
  s.buf := ptr[uint8]{0u};
  s.alloc := 0;
  s.size := 0;
};

init/allocSize ::= func(s --> string, allocSize <- uint) {
  s.buf := ptr[uint8]{/C/string/malloc(allocSize)};
  if (isNull(s.buf)) {
    s.alloc := 0u;
  } {
    s.alloc := allocSize;
  }
  s.size := 0u;
};

init/ptrSize ::= func(s --> string, cs <- ptr[uint8], len <- uint) {
  s.buf := ptr[uint8]{/C/string/malloc(len)};
  if (isNull(s.buf)) {
    s.alloc := 0u;
    s.size := 0u;
  } {
    /C/string/memcpy(s.buf, cs, len);
    s.alloc := len;
    s.size := len;
  }
}

init/cstring ::= func(s --> string, cs <- ptr[uint8]) {
  ## cs must point to a 0-terminated byte sequence
  len := var : uint{/C/string/strlen(cs)};
  init(s, cs, len);
};

init/copy ::= func(s --> strings, t <-- string) {
  init(s, t.buf, t.size);
};

## Destructor:

exit ::= func(s <--> string) {
  /C/string/free(s.buf);
  s.buf := ptr[uint8]{0u};
  s.alloc := 0u;
  s.size := 0u;
}

## Setters:

void x__strings__set__empty(s <--> string) {
  /C/stdlib/free(s.buf);
  s.buf := ptr[uint8]{0u};
  s.alloc := 0u;
  s.size := 0u;
}

void x__strings__set__ptrSize(s <--> string, m <- ptr[uint8], len <- uint) {
  if (s.alloc >= len && s.alloc <= 2 * len) {
    /C/string/memcpy(s.buf, m, len);
    s.size = len;
  } {
    tmp := var : ptr[uint8]{/C/stdlib/realloc(s.buf, len)};
    if (isNull(tmp)) {
      set(err, ALLOCATION_FAILED, ALLOCATION_FAILED_MSG);
    } {
      s.buf := tmp;
      /C/string/memcpy(s.buf, m, len);
    }
      
}

## Array access:

at_raw := func(s <-- string, pos <- uint, c -> uint8) {
  c := s.buf[pos];
};

at ::= func(s <-- string, pos <- uint, c -> uint8) [error] {
  if (pos < s.size) {
    c := s.buf[pos];
  } {
    set(error, errors/OUT_OF_BOUNDS, errors/OUT_OF_BOUNDS_MSG);
    c := uint8{0};
  }
};

## Reserve space:

reserve ::= func(s <--> string, size <- uint, iserror -> bool) [error] {
  if (s.alloc < size) {
    newSize ::= 2 * s.alloc;
    if (newSize < size) {
      newSize := size;
    }
    newbuf ::= ptr[uint8]{/C/string/realloc(s.buf, newSize)};
    if (isNull(newbuf)) {
      set(error, errors/ALLOC_FAILED, errors/ALLOC_FAILED_MSG);
      iserror := true;
    } {
      s.buf := newbuf;
      s.alloc := newSize;
      iserror := false;
    }
  }
};

## Appending:

append/string ::= func(s <--> string, t <-- string, iserror -> bool) [error] {
  iserror := false;
  if (reserve(s, s.size + t.size)) { iserror := true; return; };
  /C/string/memcpy(s.buf + s.size, t.buf, t.size);
  s.size += t.size;
};

append/char ::= func(s <--> string, c <- uint8, iserror -> bool) [error] {
  iserror := false;
  if (reserve(s, s.size + 1u)) { iserror := true; return; };
  s.buf[s.size] := c;
  s.size += 1u;
};

append/chars ::= func(s <--> string, t <- ptr[uint8], si <- uint,
                      iserror -> bool) [error] {
  iserror := false;
  if (reserve(s, s.size + si)) { iserror := true; return; }
  /C/string/memcpy(s.buf + s.size, t, si);
  s.size += si;
};

