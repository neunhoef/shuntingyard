all: strings.ll errors.ll
allo: strings.o errors.o

%.ll: %.cpp Makefile
	clang -S -emit-llvm $< -o $@ -Wall -O3 -std=c++11

strings.o: strings.h strings.cpp Makefile
	clang++ -std=c++11 -c strings.cpp -o strings.o -Wall -O3

errors.o: errors.h errors.cpp strings.h Makefile
	clang++ -std=c++11 -c errors.cpp -o errors.o -Wall -O3
