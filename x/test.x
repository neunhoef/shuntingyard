namespace := /arangodb_com/starter;
search := search + [ /google_com/v8 ];

Date := type :
  struct [
    day : int8,
    month : int8,
    year : int16
  ];

Database := type : 
  struct [
    a : int,
    b : string,
    d : Date,
  ];

move := func(from <- string, to -> string) {
  ...
}

construct := func(name <- string, db -> Database) {
  db.a := 12;
  move(name, db.b);
  db.d := Date{day := 15w8, month := 9w8, year := 1969w16};
}

Moveable := type :
  interface[
    move(
    ???
  ];

startup := func(argv <- vector[string], result -> bool) {
  t := const : int64{1000000000};   ## nanoseconds, 1s
  i := var : int{0};
  db := Database{construct("arangodb")};
  result := true;    ## all ok
  loop { if (i >= length(argv) { break }; 
    if (not(process(argv[i]))) {
      result := false;
      break;
    }
  }
};

main := func(argc <- intc, argv <- ptr[ptr[uint8]], exitcode -> intc) {
  ...
}

