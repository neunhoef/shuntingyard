all:
	./build.sh

test:	all
	build/UnitTests

clean:
	rm -rf build
