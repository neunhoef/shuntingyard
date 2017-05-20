# Reference - PUTNAMEHERE

## Overview, philosophy

## Expressions

Everything is an expression:

  - All program source code
  - All data has a representation as an expression

The syntax allows

  - atoms
  - function calls with 3 different sets of brackets and mu
  - infix operations

There is a very simple shunting yard like parser to parse source text
into an expression tree.


### What is an expression?

An expression is either an atom:

  - a typed integer of one of the following types (twos complement for signed):

      - int8
      - int16
      - int32
      - int64
      - int128
      - uint8
      - uint16
      - uint32
      - uint64
      - uint128

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

Note that the tokenizer will recognize - directly followed by a digit as a
number token and not as a token `-` followed by a number token!

Note that `#` is the comment character and is thus not available for
identifiers.

Alternatively, an expression is a **function expression**, which has

  - an identifier or operator as name (can be empty for pure bracket 
    expressions)
  - a vector of pairs (s, b), where s is a subexpression and b is a bracket
    type `empty`, `round`, `square` or `curly`.

There are three rules: 

  1. If the name is empty the vector of subexpressions must have length
     1 and must not be of empty bracket type.
  2. If the name an operator, then all pairs must have the same bracket type.
  3. If the name is an identifier, then no pair must have empty bracket type.

In this document, we denote such a tree by putting the name (or "" for empty)
in a line and the subexpressions in subsequent lines indented by a
character, and with either `-` or `(` or `[` or `{` as the first
character to indicate the bracket type. For example

    f
     (s1
     [s2
     {s3

denotes a function expression in which `f` is the name, and there are
three pairs with subexpressions s1 (round bracket type), s2 (square
bracket type) and s3 (brace bracket type).

Note that an infix notation of a function expression is also possible in the
source representation. This tree:

    +
     -a
     -*
       -b
       -c

is a tree, which would correspond to this source expression (see below):

    a + b * c

and the tree

    *
     (+
       -a
       -b
     -c

would be

    (a+b)*c

and is a tree with top name `*`, one argument pair of round bracket type and
a subtree for `a+b`, and a second argument pair of empty bracket type with
a single atom `c`.


Source representation
---------------------

Every expression tree has a canonical source representation and there
are very simple rules for parsing a source expression into an expression
tree.

Every atom has a canonical representation as an input token, for details
see the description of the tokenizer.

For a function expression canonical representation depends on the name and
there are three possibilities:

  1. The name is empty, in this case the representation is simply a pair
     of brackets of the right type around the canonical representation of
     tje single subexpression that is allowed by rule 1. above
  2. The name is an oprator, in this case the canonical representation 
     is a (potentially bracketed) infix expression of the subexpressions.
  3. The name is an identifier, in this case the canonical representation
     is one token for the identifier for the name, followed by the correctly
     bracketed subexpressions.

Example:

    ""
     (+
       -f
         (,
           x
           y
       -z

has the canonical representation

    (f(x,y)+z)

Note that due to the rules for operator precedence it is not necessarily
true that the canonical representation of an expression tree is parsed
back to the original tree! On the other hand, except for comment removal
and formatting, parsing followed by writing out the canonical representation
always returns the same expression.


## Tokenizer

The tokenizer simply scans the source in a linear fashion and uses a single
character lookahead method to find token boundaries and types.

A number is detected by a decimal digit or a - follewed by one. Here are
the rules to parse a number (TODO: explain better)

    -0..9 negative
    0x    hex
    0b    binary
    0     octal
    1..9  decimal
    . denotes float
    _ is allowed and ignored
    e or E for binary separates mantissa and exponent and means m*2^e
    e or E for octal separates mantissa and exponent and means m*8^e
    e or E for decimal separates mantissa and exponent and means m*10^e
    x or X for hexadecimal separates mantissa and exponent and means m*16^x
    w8 or w16 or w32 or w64 or w128 selects size and w64 is the default

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
    \(xyz) where xyz is any character sequence without ) and control 
           characters switches to raw mode, in which characters (including
           control characters and line breaks) are simply copied until
           the sequence xyz is found again

Examples:

    "abc\"def"
    "\(%%%)This is a multi-line
    text and is directly copied as is
    to the string constant, it can contain
    special characters like " and \ and everything,
    it is terminated by the sequence between the brackets.
    %%%

Comments: A comment starts with a `#` character and usually simply ends
at the end of the current input line (newline character). For multi-line
comments one uses two `##` characters followed by any sequence not containing
`#` or control characters, followed by a single `#` , everything up to and
including the next occurrence of the sequence between `##` and `#` is removed.

Examples:

    # this is a one line comment
    #this as well
    ##%%%# This is the start of a multi line comment,
    everything up to and including the next occurrence of the sequence
    between ## and # is removed %%%

The brackets `(`, `)`, `[`, `]`, `{` and `}` are tokens in their own right
and thus may not occur in identifiers.

As described above, an identifier consists of letters, digits,
underscores and slashes, and starts with a letter, underscore, or with a
slash directly followed by a letter or underscore.

An operator consists of characters from this set:

    !$%&'*+,-./:;<=>?@\^`|~

and is recognized if the tokenizer sees one of these symbols and it is
not one of the following cases:

  - a slash `/` followed by a letter or underscore
  - a minus `-` followed by a decimal digit

Note that

    a-b

are three tokens, whereas

    a_b

is one token, an identifier and

    a-2

is two tokens `a` and `-2`, which is probably not what is meant, one
has to write

    a - 2

or

    a- 2

instead.
    

## Parsing

The parser processes tokens in a linear fashion and maintains two stacks,
the expression stack and the function stack.

Here is an overview over the token types:

  - number
  - string
  - the six brackets `(`, `)`, `[`, `]`, `{` and `}`
  - prefix type identifier
  - infix type identifier

A number is simply put on the expression stack as an atom.

A string is simply put on the expression stack as an atom.

An identifier is put as an atom on the expression stack, if the
following token is not an opening bracket, and on the function stack,
if it is.

An operator is always put on the function stack, but before that higher
precedence operators there have to be "executed".

An opening bracket put on the function stack. A closing bracket leads to
an execute operation, which aims to assemble a complete function
expression using the tops of the two stacks.

This is all a relatively straightforward generalization of the shunting
yard algorithm. The extensions are multiple bracket types, multiple
bracket expressions in a function expression and operator precedence
rules.


## Operator precedence

    .


    ^
    * / %          left assoc
    + -            left assoc
    << >>          left assoc
    | &            left assoc
    < > <= >= =    left assoc

    || &&          left assoc
    :
    ,
    :=             left assoc
    ;


## Types and values

All types have a representation as an expression, which is

    type(TYPE)

where TYPE is replaced by what is described below.

Scalar:

int8
int16
int32
int64
int128
uint8
uint16
uint32
uint64
uint128
float32
float64
string
bool                 uses 1 byte in structs
niltype              uses 0 bytes in structs

Expressions:

expr 
type                 represented as an expression

Composite types:

array[3].TYPE
struct {             packing is well-defined
  IDEN : TYPE,
  IDEN : TYPE,
  NAME(4) : TYPE,    4 is the byte offset from the beginning of the structure
}

Indirect ones:

ptr{TYPE}
slice{TYPE}
vector{TYPE}        holds allocation
map{TYPE -> TYPE}

Function types (by means of example):

func(IDEN <- TYPE, ...)[...]...

(an arbitrary sequence of round and square bracket arguments each containing
either 

  nothing, or
  IDEN <- TYPE, or
  IDEN -> TYPE, or
  IDEN <-> TYPE, or
  a non-empty comma expression of any of the previous three

EBF:

FUNCTYPE := "func" ARGLIST+
ARGLIST :=   "()" 
           | "[]"
           | "(" INNERARGLIST ")"
           | "[" INNERARGLIST "]"
INNERARGLIST := ARG ("," ARG)*
ARG :=   IDEN "<-" TYPE
       | IDEN "->" TYPE
       | IDEN "<->" TYPE

Note that for a function type there may not be a curly brace expression,
because this denotes the body of a function definition.

For every type there is a representation of its objects as an expression.
The scalar ones are clear (using the reserved identifiers nil, false and
true) and number and string literals. expr and type are expressions. For
arrays we use the reserved function names array[...] and struct[IDEN:TYPE,...].
Likewise, ptr{TYPE}(ADDRESS) and slice{TYPE}(ADDRESS, SIZE, CAPACITY) and
vector[...] and map["a" -> 1, "b" -> 2].

A named type is expressed as

type("auintvec", array[8].uint16)


## Data Literals


## Evalation of expressions

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

