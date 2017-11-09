all: strings.ll errors.ll tokenizer.ll tokenizer_test tokenizer_guck
allo: strings.o errors.o tokenizer.o

%.ll: %.cpp Makefile
	clang -S -I. -emit-llvm $< -o $@ -Wall -O3 -std=c++11

strings.o: strings.h strings.cpp Makefile
	clang++ -std=c++11 -I. -c strings.cpp -o strings.o -Wall -O0 -g

errors.o: errors.h errors.cpp strings.h Makefile
	clang++ -std=c++11 -I. -c errors.cpp -o errors.o -Wall -O0 -g

tokenizer.o: tokenizer.h tokenizer.cpp strings.h errors.h Makefile
	clang++ -std=c++11 -I. -c tokenizer.cpp -o tokenizer.o -Wall -O0 -g

tokenizer_test.o: tokenizer.h strings.h errors.h Makefile tokenizer_test.cpp
	clang++ -std=c++11 -I. -c tokenizer_test.cpp -o tokenizer_test.o -Wall -O0 -g

tokenizer_test: tokenizer_test.o strings.o errors.o tokenizer.o
	clang++ -o tokenizer_test tokenizer_test.o strings.o errors.o -g \
		tokenizer.o

tokenizer_guck.o: tokenizer.h strings.h errors.h Makefile tokenizer_guck.cpp
	clang++ -std=c++11 -c tokenizer_guck.cpp -o tokenizer_guck.o -Wall -O0 -g

tokenizer_guck: tokenizer_guck.o strings.o errors.o tokenizer.o
	clang++ -o tokenizer_guck tokenizer_guck.o strings.o errors.o \
		tokenizer.o -g

clean:
	rm -rf *.o *.ll tokenizer_test tokenizer_guck
