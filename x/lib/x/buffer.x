## Buffer implementation

namespace := /x/buffer;
search := [ /x/ ];

Buffer ::= type : struct[
  start : ptr[uint8],
  alloc : uint32,
  used  : uint32,
];



