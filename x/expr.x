## Handling of expressions

namespace := /x/internal/expressions;
search := [ /x/internal/types ];

AtomType := type(int8);


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
    ];
    bra : BracketType;
    sub : vector[ExprNode]
  ]);

expr := type : ref[exprnode];

exprnode ::= type : refcounted[atom | molecule];

atomtype := type :
  enum[int32]{
    PosInt := 0;
    NegInt := 1;
    UInt := 2;
    Float := 3;
    String := 4;
  };

atom := type :
  struct[
    offset(0)[
      i := int64;
      u := uint64;
      f := float64;
      s := box[string];
    ];
    type := atomtype;
  ];

bracket := type : 
  enum[int32]{
    None := 0;
    Round := 1;
    Square := 2;
    Curly := 3;
  };

molecule := type :
  struct[
    nam := string;
    bra := bracket;
    sub := vector[expr];
  ];



     {
       e := var : expr;

       addr(x);
       follow(x);
       match(x := e)){
         atom {
           ## x is e as a ref[atom] in this block
         };
         molecule {
           ## x is e as a ref[molecule] in this block

         };
       }
         
       match(e) {
         atom {
         };
         molecule {
         };
       }
