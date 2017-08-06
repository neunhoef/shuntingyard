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

isAtom := func(e <- expr, b -> bool);
atomType := func(e <- expr, t -> AtomType);  ## only for atoms
toString := func(e <- expr, s -> string);    ## only for atoms, else empty

isFunc := func(e <- expr, b -> bool);
isOper := func(e <- expr, b -> bool);        ## isOper implies isFunc
name := func(e <- expr, s -> string);        ## only for isFunc true
length := func(e <- expr, l -> uint);        ## always positive

