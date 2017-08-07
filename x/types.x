## Dummy file to record builtin types

namespace := /x/internal/types;

int8 := type("int8");
int16 := type("int16");
int32 := type("int32");
int64 := type("int64");
intp := type("intp");     ## same bit width as a pointer
intc := type("intc");     ## same bit width as a C int
intl := type("intl");     ## same bit width as a C long int

uint8 := type("uint8");
uint16 := type("uint16");
uint32 := type("uint32");
uint64 := type("uint64");
uintp := type("uintp");   ## same bit width as a pointer
uintc := type("uintc");   ## same bit width as a C int
uintl := type("uintl");   ## same bit width as a C unsigned long int

float32 := type("float32");
float64 := type("float64");

string := type("string");

bool := type("bool");   ## values true and false, uses 1 byte in structs
true := true;
false := false;

niltype := type("niltype");
nil := nil;

expr := type("expr");
type := type("type");

## Composite types:

## ptr ^ ...         pointer to some type
## box ^ ...         unique_ptr
## ref ^ ...         reference counting pointer
## slice ^ ...       pointer and length
## vector ^ ...      slice keeping allocation
## map : ... -> ...  associative array
## struct{...}       structure
## array[...] ^ ...  compile time fixed length

## Functions:

## func(IDEN <- TYPE, ...)
## func(IDEN <- TYPE, ...){
##   ...
## }

