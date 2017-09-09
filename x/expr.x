## Handling of expressions

namespace := /x/internal/expressions;
search := [ /x/internal/types ];

AtomType := type(int8);

AtomTypeNil    := AtomType(0w8);
AtomTypeInt8   := AtomType(1w8);
AtomTypeInt16  := AtomType(2w8);
AtomTypeInt32  := AtomType(3w8);
AtomTypeInt64  := AtomType(4w8);
AtomTypeUInt8  := AtomType(5w8);
AtomTypeUInt16 := AtomType(6w8);
AtomTypeUInt32 := AtomType(7w8);
AtomTypeUInt64 := AtomType(8w8);
AtomTypeFloat32 := AtomType(9w8);
AtomTypeFloat64 := AtomType(10w8);
AtomTypeString := AtomType(11w8);
AtomTypeIdent  := AtomType(12w8);

BracketType := type(int8);

BracketTypeEmpty := BracketType(0w8);
BracketTypeRound := BracketType(1w8);
BracketTypeSquare := BracketType(2w8);
BracketTypeCurly := BracketType(3w8);

isAtom := func(e <- expr, b -> bool);
atomType := func(e <- expr, t -> AtomType);  ## only for atoms
toString := func(e <- expr, s -> string);    ## only for atoms, else empty

isFunc := func(e <- expr, b -> bool);
isOper := func(e <- expr, b -> bool);
name := func(e <- expr, s -> string);
bracketType := func(e <- expr, t -> BracketType);
length := func(e <- expr, l -> uintp);
at := func(e <- expr, pos <- uintp, res -> expr);

ExprNode := type(
  struct [
    offset(0)[
      i : int64,
      u : uint64,
      s : float(32),
      d : float(64),
      p : ptr[niltype]
    ],
    bra : BracketType;
    sub : vector[ExprNode]
  ]);

expr := type(ref[ExprNode]);
