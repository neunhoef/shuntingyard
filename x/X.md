# Reference - X

## Overview, philosophy

The fundamental new idea of X is that there is the syntax for
expressions, defined by a relatively simple and yet powerful parser that
transforms a textual expression into an expression tree. In X,
everything is an expression:

  - all program source code
  - all data has a canonical representation as an expression
  - types have a canonical representation as an expression

Since X code can readily use the parser, manipulate expression trees
dump expression trees back into textual form, a lot of features that
are more complicated can be implemented in X itself as libraries
extending the core language:

  - syntax checks (higher level than parsing)
  - templates
  - macros
  - linting
  - borrow checking
  - inheritance
  - static analysis

Furthermore, since X code can be interpreted or compiled, meta
programming is built into the language and is a lot more comprehensible
and similar to "normal" programming.

## Expressions

Everything is an expression:

  - all program source code
  - all data has a canonical representation as an expression
  - types have a canonical representation as an expression
  - expressions can be used to implement DSLs

The syntax allows

  - atoms (numbers, strings, identifiers)
  - function calls
  - infix operations
  - arbitrary UTF-8 strings embedded in the source
  - comments

There is a very simple shunting yard like parser to parse source text
into an expression tree.


### What is an expression?

An expression is either an atom:

  - a typed integer of one of the following types (twos complement for signed):

      - int8
      - int16
      - int32
      - int64
      - int      (same width as a pointer)
      - intc     (same width as a C int)
      - intl     (same width as a C long int)
      - uint8
      - uint16
      - uint32
      - uint64
      - uint     (same width as a pointer)
      - uintc    (same width as a C unsigned int)
      - uintl    (same width as a C unsigned long int)

  - a typed floating point number of one of the following types:

      - float32
      - float64

  - a string constant (UTF-8, indicated by a `"` character)

  - an identifier, which can either consist of characters from this set:

        A..Z a..z _ 0..9 /

    and begin with a letter, an underscore or with / directly followed 
    by a letter or underscore

  - an operator, which can consist of characters from this set:

        !$%&'*+,-./:;<=>?@\^`|~

Note that the tokenizer will recognize + or - directly followed by a
digit as a number token and not as a token `-` followed by a number
token!

Note that `#` is the comment character and is thus not available for
identifiers and operators.

Note that `/` is a valid identifier character and thus `a/b` is a single
token. For division one has to write `a / b` instead.

Alternatively, an expression is a **function expression**, which has

  - a name (which is a UTF-8 string),
  - a bracket type `empty`, `round`, `square` or `curly`, and
  - a vector of subexpressions

The name can be

  - an **identifier** (must be non-empty), in which case it is a prefix
    function expression
  - an **operator** (must be non-empty), in which case it is an infix
    operator expression
  - **empty**, in which case it is merely used for the bracketing

There are two rules: 

  1. If the name is empty, the bracket type must not be empty and the vector
     of subexpressions must have length 1
  2. If the name is an identifier, then all subexpressions must have
     non-empty bracket type.

In this document, we denote such a tree by putting the name (or "" for
empty) in a line, with the right brackets around it (or none) and the
subexpressions in subsequent lines indented by a character.

    f
     s1
     s2
     s3

denotes a function expression in which `f` is the name, and there are
three subexpressions s1, s2 and s3.

Note that an infix notation of a function expression is also possible in the
source representation. This tree:

    +
     a
     *
      b
      c

is a tree, which would correspond to this source expression (see below):

    a+b*c

and the tree

    *
     (+)
      a
      b
     c

would be

    (a+b)*c

and is a tree with top name `*`, one argument of round bracket type and
a subtree for `a+b`, and a second argument pair of empty bracket type with
a single atom `c`.


Source representation
---------------------

Every expression tree has a canonical source representation and there
are very simple rules for parsing a source expression into an expression
tree.

Every atom has a canonical representation as an input token, for details
see the description of the tokenizer.

For a function expression the canonical representation depends on the name
and there are three possibilities:

  1. The name is empty, in this case the representation is simply a pair
     of brackets of the right type around the canonical representation of
     the single subexpression that is allowed by Rule 1 above
  2. The name is an operator, in this case the canonical representation 
     is a (potentially bracketed) infix expression of the subexpressions.
  3. The name is an identifier, in this case the canonical representation
     is one token for the identifier for the name, followed by the correctly
     bracketed subexpressions.

Examples:

    (+)
     f
      (,)
       x
       y
     z

has the canonical representation

    (f(x,y)+z)

    +
     a
     *
      b
      c

has the canonical representation

    a+b*c

whilst

    *
     (+)
      a
      b
     c

has

    (a+b)*c

and

    *
     a
     (+)
      b
      c

has

    a*(b+c)

and

    +
     *
      a
      b
     c

has

    a*b+c

Note that due to the rules for operator precedence the canonical
representation of a tree is always parsed back to the original tree, and
except for comment removal and formatting, parsing followed by writing
out the canonical representation always returns the same expression.


## Tokenizer

The tokenizer simply scans the source in a linear fashion and uses a single
character lookahead method to find token boundaries and types.

A number is detected by a decimal digit or a sign (+ or -) follewed by one.
Here are the rules to parse a number (TODO: explain better)

    +0..9 positive signed
    -0..9 negative signed
    0..9  unsigned
    0x    hex
    0b    binary
    0o    octal
    0..9  decimal if second letter is neither `x`, `b` or `o`
    . denotes float and serves as decimal point
    _ is allowed and ignored (whitespace within numbers is not!)
    *2^   stops the mantissa and starts an exponent with basis 2
    *8^   stops the mantissa and starts an exponent with basis 8
    *10^  stops the mantissa and starts an exponent with basis 10
    *x^  stops the mantissa and starts an exponent with basis 16

The result is either int64 for signed or uint64 for unsigned or float64.
One can express a constant of a different type by using the notation

    uint32{17}

for example. Integer overflow whilst scanning makes the value equal to 0.

A string constant is recognized by the character `"`. Here are the
rules to parse a string (TODO: explain better)

    ended by `"` not preceded by a `\`
    \t    is a tab
    \n    is a newline
    \r    is a carriage return
    \\    is a backslash
    newlines and carriage returns in strings contants are not allowed
    \xXX   is a single character in the 0x00-0x7f ASCII range
    \uXXXX is a single unicode character
    \UXXXXXXXX is a single unicode character
    \"    is a double quote
    \|xyz| where xyz is any character sequence without | and control 
           characters switches to raw mode, in which characters (including
           control characters and line breaks) are simply copied until
           the sequence |xyz| is found again
    \c     anywhere in the string switches to character mode,
           the result of the token is not a string token but rather
           an uint64 (less than 2^32) that is the value of the first
           code point in the string result

**Examples**:

    "abc\"def"
    "\|%%%|This is a multi-line
    text and is directly copied as is
    to the string constant, it can contain
    special characters like " and \ and everything,
    it is terminated by the sequence between the brackets.
    |%%%|"
    "\cA" is the same as uint32{65}


**Comments**: There are two forms for comments, one starts with two `##`
characters and simply ends at the end of the current input line (newline
character). For multi-line comments one uses two `#` characters with a
non-empty sequence not containing `#`, space or control characters
between the two `#` characters. Everything up to and including the next
occurrence of the sequence between the `#` is removed.

**Examples**:

    ## this is a one line comment

    ##this as well

    #%%%# This is the start of a multi line comment,
    everything up to and including the next occurrence of the sequence
    between ## (including the ##) and # is removed #%%%#

    #...# This is also a start
    and continues until the next occurrence of three dots is found
    ...## the following ## removes the rest of the line

    This can be used to comment out something #%# within a line %.

The brackets `(`, `)`, `[`, `]`, `{` and `}` are tokens in their own right
and thus may not occur in identifiers or operators.

As described above, an identifier consists of letters, digits,
underscores and slashes, and starts with a letter, underscore, or with a
slash directly followed by a letter or underscore.

An operator consists of characters from this set:

    !$%&'*+,-./:;<=>?@\^`|~

and is recognized if the tokenizer sees one of these symbols and it is
not one of the following cases:

  - a slash `/` followed by a letter or underscore
  - a minus `-` followed by a decimal digit
  - a plus `+` followed by a decimal digit

Note that

    a-b

are three tokens, whereas

    a_b

is one token, an identifier, and

    a-2

is two tokens `a` and `-2`, which is probably not what is meant, one
has to write

    a - 2

or

    a- 2

instead. Furthermore, note that

    a/b

is one token and division of `a` by `b` must be denoted by

    a / b
    

## Parsing

The parser processes tokens in a linear fashion and maintains two stacks,
the expression stack and the function stack.

Here is an overview over the token types:

  - number
  - string
  - the six brackets `(`, `)`, `[`, `]`, `{` and `}`
  - prefix type identifier (function name)
  - infix type identifier (operator name)

A number is simply put on the expression stack as an atom.

A string is simply put on the expression stack as an atom.

An identifier is put as an atom on the expression stack, if the
following token is not an opening bracket, and on the function stack,
if it is.

An operator is always put on the function stack, but before that higher
precedence operators there have to be "executed".

An opening bracket is put on the function stack. A closing bracket leads to
an execute operation, which aims to assemble a complete function
expression using the tops of the two stacks.

This is all a relatively straightforward generalization of the shunting
yard algorithm. The extensions are multiple bracket types, multiple
bracket expressions in a function expression and operator precedence
rules.


## Operator precedence

    .                                           140

    ^ ^^ ^^^                   right assoc      130
    * / %                      left assoc       120
    + -                        left assoc       110
    | &                        left assoc       100
    << >>                      left assoc       90
    < > <= >= = !=             left assoc       80
    ..                         left assoc       70
    || &&                      left assoc       60

    -> <- <-> --> <-- <-->     left assoc       50
    : ::                       left assoc       40
    ,                                           30
    := ::=                     left assoc       20
    ;                                           10


## Types and values

All types have a representation as an expression, which is either

    type(NAME := TYPE)

for a named type or

    type(TYPE)

for an anonymous or internal type, where TYPE is replaced by what
is described below.

**Scalar**:

    int8
    int16
    int32
    int64
    int
    intc
    intl
    uint8
    uint16
    uint32
    uint64
    uint
    uintc
    uintl
    float32
    float64
    string
    bool                 uses 1 byte in structs
    niltype              uses 0 bytes in structs

**Expressions**:

    expr 
    type                 represented as an expression

**Composite types**:

    array[3][TYPE]

    struct [           packing is well-defined
      IDEN : TYPE,
      IDEN : TYPE,
      offset(4),       4 is the byte offset from the beginning of the structure
      IDEN : TYPE,
     public,
      IDEN2 : TYPE,    from the word "public" on everything is visible
     private,
      IDEN3 : TYPE     from the word "private" on everything is private
    ]

Instead of an unsigned numerical value like 4 above one can specify the
identifier of a previously mentioned field like so (to make a union of
a uint64 and a float64:

    struct [
      a : uint64,
      offset(a),
      b : float64,
    ]

**Indirect types**:

Plain pointer to TYPE:

    ptr[TYPE]

Smart pointer with automatic deletion but no reference counting:

    box[TYPE]

Smart pointer with automatic reference counting:

    ref[TYPE]

needs to point to a

    refcounted[TYPE]

Arrays:

    array[SIZE][TYPE]

Slices with a pointer and a length:

    slice[TYPE]

Vector with a pointer, a length and a capacity with automatic deletion:

    vector[TYPE]        holds allocation

    map[TYPE -> TYPE]

**Algebraic types**:

    variant[TYPENAME | TYPENAME | TYPENAME | ... | TYPENAME]

**Function types (by means of example)**:

    func(IDEN <- TYPE, ...)

(one subexpression with round bracket type which is either empty, a
single subexpression as below or a comma expression with a sequence of
non-empty expressions of the following shape:

    IDEN <- TYPE, or
    IDEN -> TYPE, or
    IDEN <-> TYPE, or
    IDEN <-- TYPE, or
    IDEN --> TYPE, or
    IDEN <--> TYPE

**EBF**:

    FUNCTYPE := "func" ARGLIST
    ARGLIST :=   "(" ARG ("," ARG)* ")"
               | "(" ")"
    ARG :=   IDEN "<-" TYPE
           | IDEN "->" TYPE
           | IDEN "<->" TYPE
           | IDEN "<--" TYPE
           | IDEN "-->" TYPE
           | IDEN "<-->" TYPE

Note that for a function type there may not be a curly brace expression,
because this denotes the body of a function definition.

Interface types:

    interface(T)[
       ...
    ]

## Data Literals

For every type there is a representation of its objects as an expression.
The scalar ones for builtin types are clear (using the reserved identifiers
nil, false and true) and number and string literals. For the C integers
`intp`, `intc`, `intl`, `uintp`, `uintc`, `uintl` one has to use 
`intp{10}` and so on, already using the brace syntax.
expr and type are expressions. For all other types we use in a similar
way the type name with a brace expression appended.

Examples:

    array[3][int64]{1, 2, 3}
    ptr[int64]{0x12345678}
    box[int64]{0x12345678}
    ref[int64]{0x12345678}
    slice[float64]{0x12345678, 12}
    vector[float64]{1.0, 2.0, 3.0}
    struct[x : float64, y : float64]{x := 1.0, y := 2.0}


A named type is expressed as

    type(auintvec := array[8][uint16])

An expression is nothing but a reference counting pointer to an
`ExprNode`:

    /x/internal/types/expr := type(ref[ExprNode]);


## Evaluation of expressions

Here are the evaluation rules by means of example, E and E1 and E2 and ...
are place holders for arbitrary expressions to formulate the rules:

eval(1)

    1 : int64

eval("abc")

    "abc" : string

eval(true)

    true : bool

eval(nil)

    nil : niltype

eval(expr(E))

    E : expr

eval(expr(E)[E2])

    illegal

eval(abc)

  if "abc" is bound to value V and type T, then this evaluates to

    V : T

  otherwise an error occurs. Note that this can be decided at compile time!

eval(E + E1)

  if eval(E) and eval(E1) evaluate to types T and T1 and there is
  a function bound to "+" with type

    func(a <- T, b <- T1, c -> T2)

  then this function is called with eval(E) for a and eval(E1) for b
  and the whole expression evaluates to

    c : T2

eval(f(E)[E1](E2))

  if "f" is bound to a function with type matched by the types in
    (eval(E))[eval(E1)](eval(E2))
  then f is called and the result is returned with the appropriate type.


## Name spaces

There is a hierarchical tree of namespaces. Each input file ...


## Function bodies

The body of a function is a semicolon-separated list of expressions in
curly brackets:

     func(a <- int64, b <- int64, r -> int64) {
       r := a + b;
     }

The return statement does not have an argument, one simple assigns to
output arguments. An empty subexpression is allowed (as for example here
the last one) and is a no-operation. Assignment takes a comma-separated
list of L-values on the left of `:=` and a comma-separated list of
expressions on the right. The semantic is to evaluate all expressions
on the left and then assign the results to the list of L-values on the
left.

Example:

    a, b, c := f(x), g(y);
    ## if for example f returns 2 values and g one

If statements:

    if (CONDITION) {
      ...
    } (CONDITION2) {    ## elseif part with another condition
      ...
    } {                 ## else part
      ...
    }

The `elseif` and `else` parts are optional. Round brackets and Braces
must not be omitted.

This means that a switch can be done as follows:

    x := someExpression;
    if (x == 1) {
      ## do something
    } (x == 2) {
      ## do something else
    } (x == 3) {
      ## do something else
    } {
      ## default action
    }

The compiler can itself find out that this is a switch.

Loops:

    name : loop (CONDITION) {
      ...
      if (CONDITION2) {
        ...
        break(name)
      };
      if (CONDITION3) {
        continue(name)
      };
      if (CONDITION4) { break };
      if (CONDITION5) { continue };
      ...
    };

The `name:` label part is optional, as is the `(name)` part in the
`break` and `continue` statements. The `(CONDITION)` part is optional
but, if present, is evaluated before entering the loop.

Variables:

Local variables are declared as follows:

    i := var : int64;            ## Uninitialized
    d := var : float64{12.0};    ## Initialized, could have been
    o := var : ComplicatedObject{construct(12, "abc")};   ## initializer call
    u := var : SomeObject;       ## Default init() is called and must exist

Local variables are automatically destructed when going out of scope
with `exit` being called on them.

For a type T, `exit` always has this signature:

    destruct(this <-- ptr[T])

Each constructor `init`for a type T has as first argument a
`--> T`, usually called `this`. This type (and possibly others
following) decides which constructor to take.

Global variables are done in the same way. For initialization order see
below.

Constants are done similarly (must always be initialized):

    c := const : float64{1.0};

Local types:

    t := type : 

Catching exceptions:

    try {
      ...
    } (e : TYPE) {
      ...
    } {
      ...
    }


NO LONGER TRUE:

Here is a coroutine:

    c := func(from <- int32, to <- int32, val -> int32, done -> bool) {
      i := from;        ## this declares i as variable of type int32
      loop (i <= to) {
        val := i;
        done := false;
        yield;
        i := i + 1;
      }
      done := true;
    }

And here is how to use it:

    f := func() {
      v := var : int32;
      v := var : int32;
      d := var(bool{false});
      scope {
        h := var(coroutine{c(10w32, 20w32)});
        loop {
          v, d := h();
          if (not(d)) { break };
          /x/io/writeln(v);
        }
      }
      ## When h goes out of scope, the coroutine is shut down.
    }

## Objects, constructors, destructors, allocators and freeers

Semantics of local variables on the stack:

    f := func(a <- int) {
      o := var : mytype{cons(1, 2, "abc")};
      ...
      o2 := var : mytype{cons(2, 3)};
      ...
      o3 := var : ptr[mytype]{alloc(3, 4)};
      ...
      if (a == 1) {
        ...
        return
      }
      ...
    }

When execution reaches o, a new variable named "o" comes into scope, it
is of type "mytype" and the constructor with arguments 1, 2 and "abc" is
called. Similarly with o2. For o3, a new value of type pointer to mytype
comes into scope. It is initialized as a pointer to a newly allocated
structure of type "mytype" or a null pointer if allocation failed. Then
the constructor "cons" is called with arguments 3 and 4.

If there is an explicit definition of a function alloc like this exists:

    func alloc(res -> ptr[mytype])

or

    func alloc(nr <- uint, res -> ptr[mytype])

then this is called, it must allocate memory for a "mytype" object and
then the constructor must construct the object at the same time and return the pointer.

There is also

    func free(p <- ptr[mytype])

If the scope of any of these is left (return statement or end of
function), then nothing happens for o3, but first for o2 and then
for o the destructor "dest" is called, which must be a function with 
this signature:

    func dest(o <-> mytype)

## Generic types

The following generic types are built in the language:

### Dumb pointer: `ptr[T]`

This is a pointer to an object of type T. No special magic, the
constructor can take a `uint` as address or nothing, in which case
the pointer is null.

Generic function are:

    cons := func(p <-> ptr[T]);
    cons := func(p <-> ptr[T], a <- uint);
    dest := func(p <-> P);
    isNull := func(p <- ptr[T], res -> bool);
    ":=" := func(p <-> P, q <- P);
    "+" := func(p <- ptr[T], steps <- int, res -> ptr[T]);
    "-" := func(p <- ptr[T], steps <- int, res -> ptr[T]);
    "-" := func(p <- ptr[T], q <- ptr[T], res -> int);
    address := func(p <- ptr[T], res -> uint);

Since we know for every type T how many bytes it needs, this can all
be implemented generically.

That is, a dumb pointer implements the IsPointer interface:

    PointerInterface := interface(P) {
      cons := func(p <-> P);
      cons := func(p <-> P, a <- uint);
      dest := func(p <-> P);
      isNull := func(p <- P, res -> bool);
      ":=" := func(p <-> P, q <- P);
      "+" := func(p <- P, steps <- int, res -> P);
      "-" := func(p <- P, steps <- int, res -> P);
      "-" := func(p <- P, q <- P, res -> int);
      address := func(p <- P, res -> uint);
    };

### Automatic pointers: `box[T]`

This is similar to a `unique_ptr` in C++, it is a pointer that guards
the allocation of the object it is pointing to.

Only the destructor is different, it automatically calls the destructor
of the object being pointed to, and frees the allocation.

### Reference counting pointers: `ref[T]`

This is similar to a `shared_ptr` in C++, it is a pointer that guards
the allocation of the object it is pointing to. It does so in a
multi-threaded way using an atomic uint reference counter in the first
few bytes of the object pointed to. Therefore, the type T *must*
actually be refcounted[S] for some type S, which adds the atomic uint
in front of the object.


## Access to struct components

**Rule**: There are public and private components, a private component
can only be accessed from a function which receives the variable with
the long arrow notation (see below), or in construction using a struct
literal. A public component is declared with two :: characters instead
of one.

Examples:

    T := type : struct[
      x : int64;
      y :: int64;
    ];

    init := func(t --> T, xx <- int64, yy <- int64) {
      t.x := xx;   ## allowed because of long arrow for t
      t.y := yy;   ## allowed also because of public y
    }

    b := func(t <--> T) {
      # Access allowed because of long arrow
    }

    a := func() {
      A := var : T{x := 1, y := 2};   ## allowed because of struct literal
      B := var : T{cons(3, 4)};       ## allowed because of constructor
      A.y := 12;                      ## allowed because of public
    }

A function argument can be an input, an output, or a reference, which is
denoted by

    NAME <- TYPE          for an input argument
    NAME -> TYPE          for an output argument
    NAME <-> TYPE         for a reference argument

Inputs are immutable, output arguments count as uninitialized but can
be written to. Reference arguments count as initialized and can be
modified. If the TYPE is a struct type, then the above notation does not
allow the function to inspect or modify the private member variables. If
the function shall have access to the member values, one has to write
two minus signs as in (long arrow notation)

    NAME <-- TYPE         to give the function read access to members
    NAME --> TYPE         to give the function write access to members
    NAME <--> TYPE        to give the function read/write access to members

A normal function taking two immutable inputs and a single result
without member access:

    sale ::= func(a <- int, b <- int, c -> int);

A constructor is called `init` and has the type it is constructing as
the first output argument (here the type is `User`):

    init  ::= func(u --> User, name <- string, firstName <- string)

A destructor is called `exit` and has the type it is destructing as
the only argument which is a reference:

    exit := func(u <--> User);

An allocator for a type `User` has one of the following two signatures:

    alloc/single := func(u -> ptr[User]);
    alloc/array := func(u -> ptr[User], nr <- uint);     (for arrays)

If not declared, a default allocator is automatically generated using
malloc.

A free for a type `User` has the following signature:

    free := func(u <- ptr[User]);

It is automatically generated by the compiler unless one defines it.

Here is a table showing how arguments of a function can be passed on,
the row says how an argument was passed into the function, the column
says, how the argument can be passed on to a called function:

    argument    <-      <->     ->
    <-          +       -       -
    <->         +       +       (**)
    ->          (*)     (*)     (***)
            (*) means after initialization
            (**) after destruction
            (***) only before initialization

## Dictionary C <-> X

    int* p;                             p := var : ptr[int];
    p = q;                              p := q;
    i = *p;                             i := p[0];
    Pos* x;                             x := var : ptr[Pos];

    struct Pos {                        Pos := type : struct[
      double x;                           x : float64;
      double y;                           y : float64;
    };                                  ];
    Pos* p;                             p := var : ptr[Pos];
    p->x                                p^x
    p = q;                              p := q;
    *p                                  p[0]
    Pos pos;                            pos := var : Pos;
    p = &pos;                           p := addr(pos);
    Pos** q;                            q := var : ptr[ptr[Pos]];
    **q                                 q[0][0]
    (*(q[12])).x or q[12]->x            q[12][0].x or q[12]^x
    q[13] = &pos;                       q[13] := addr(pos);
    (**q).x  or  (*q)->x                q^^x
    Pos*** r;                           r := var : ptr[ptr[ptr[Pos]]];
    ***r                                r[0][0][0]
    (***q).x   or (**q)->x              r[0][0][0].x  or  q[0][0]^x


## Initialization order of global variables

Each namespace can declare that its initialization ought to happen
after some other namespace as follows:

    namespace := /a/b/c;

    initizalize_after(/a/b/d);
    initizalize_after(/e);

The compiler generates an initialization function for each such
namespace and a registration function. At startup all registration
functions will be executed, then a correct order of initialization
is determined respecting all the `initialize_after` statements and
if this does not work the program is terminated.

Otherwise, the initialization functions are executed one after another
in the determined order.

INVESTIGATION: How well does this plan play with the linker on various
platforms???
