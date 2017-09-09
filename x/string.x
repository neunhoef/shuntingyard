#---# Strings
This library implements strings as UTF-8 byte sequences. It underpins
the builtin type /x/internal/strings/string.
---

namespace := /x/internal/strings;
search := search + [ /C/stdlib/ ]

string ::= type(
  struct[
    buf   : ptr[uint8],
    alloc : uint,
    len   : uint
  ]
);

## Constructors for a string:

construct :== func(s <-> string) {
  s.buf := ptr[uint8]{0u};
  s.alloc := 0;
  s.len := 0;
}

construct :== func(s <-> string, allocSize <- uint) {
  s.buf := ptr[uint8]{/C/stdlib/malloc(allocSize)};]
  if (isNull(s.buf)) {
    s.alloc := 0;
  } {
    s.alloc := allocSize;
  }
  s.len := 0;
}

construct ::= func(s <-> string, cs <- ptr[uint8]) {
  ## cs must point to a 0-terminated byte sequence
  len := var(uint, /C/string/strlen(cs));
  construct(s, cs, len);
}

construct :== 
