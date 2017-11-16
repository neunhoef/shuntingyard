#---# string.x - Strings
This library implements strings as UTF-8 byte sequences. It underpins
the builtin type /x/string.
#---#

namespace := /x;
search := [/x/errors];

string ::= type :
  struct [
    buf   : ptr[uint8];
    alloc : uint;
    size  : uint;
  ];
);

## Constructors:

init/empty ::= func(s --> string) {
  s.buf := ptr[uint8]{0};
  s.alloc := 0;
  s.size := 0;
};

init/allocSize ::= func(s --> string, allocSize <- uint) {
  s.buf := ptr[uint8]{/C/string/malloc(allocSize)};
  if (isNull(s.buf)) {
    s.alloc := 0;
  } {
    s.alloc := allocSize;
  }
  s.size := 0;
};

init/ptrSize ::= func(s --> string, cs <- ptr[uint8], len <- uint) {
  s.buf := ptr[uint8]{/C/string/malloc(len + 1)};
  if (isNull(s.buf)) {
    s.alloc := 0;
    s.size := 0;
  } {
    /C/string/memcpy(s.buf, cs, len);
    s.buf[len] := uint8{0};
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
  s.buf := ptr[uint8]{0};
  s.alloc := 0;
  s.size := 0;
}

## Setters:

set/empty := func(s <--> string) {

  /C/stdlib/free(s.buf);
  s.buf := ptr[uint8]{0};
  s.alloc := 0;
  s.size := 0;
}

set/ptrSize := func(s <--> string, m <- ptr[uint8], len <- uint) {
  if (s.alloc >= len+1 && s.alloc <= 2 * len) {
    /C/string/memcpy(s.buf, m, len);
    s.buf[len] := uint8{0};
    s.size = len;
  } {
    tmp := var : ptr[uint8]{/C/stdlib/realloc(s.buf, len)};
    if (isNull(tmp)) {
      set(err, ALLOCATION_FAILED, ALLOCATION_FAILED_MSG);
    } {
      s.buf := tmp;
      /C/string/memcpy(s.buf, m, len);
      s.buf[len] := 0;
      s.size = len;
    }
  }
}

set/string ::= func(s <--> string, t <-- string) {
  set(s, t.buf, t.size);
}

## Array access:

c_str ::= func(s <-- string, r -> ptr[uint8]) {
  r := s.buf;
}

size ::= func(s <-- string, r -> uint) {
  r := s.size;
}

at_raw ::= func(s <-- string, pos <- uint, c -> uint8) {
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

reserve ::= func(s <--> string, size <- uint) [error] {
  if (s.alloc < size+1) {
    newSize ::= 2 * s.alloc;
    if (newSize < size+1) {
      newSize := size+1;
    }
    newbuf ::= ptr[uint8]{/C/string/realloc(s.buf, newSize)};
    if (isNull(newbuf)) {
      set(err, errors/ALLOC_FAILED, errors/ALLOC_FAILED_MSG);
    } {
      s.buf = newbuf;
      s.alloc = newSize;
    }
  }
};

## Comparison:

equals ::= func(s <-- string, t <-- string) {
  
};

equals/c_str ::= func(s <-- string, t <- ptr[uint8], size <- uint) {
  
};

## Appending:

append/string ::= func(s <--> string, t <-- string) [error] {
  reserve(s, s.size + t.size); if (err) { return };
  /C/string/memcpy(s.buf + s.size, t.buf, t.size);
  s.size += t.size;
};

append/char ::= func(s <--> string, c <- uint8) [error] {
  reserve(s, s.size + 1); if (err) { return };
  s.buf[s.size] := c;
  s.size += 1;
  s.buf[s.size] := uint8{0};
};

append/chars ::= func(s <--> string, t <- ptr[uint8], si <- uint) [error] {
  reserve(s, s.size + si); if (err) { return };
  /C/string/memcpy(s.buf + s.size, t, si);
  s.size += si;
  s.buf[s.size] := uint8{0};
};

