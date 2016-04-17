CXXFLAGS = -O2 -Wall -Werror -std=c++11

.PHONY: clean all
all: bin/main
bin/main: bin/main.o bin/decompress.o bin/compress.o bin/parse.o bin/node.o
	g++ $(CXXFLAGS) bin/main.o bin/decompress.o bin/compress.o bin/parse.o bin/node.o -o bin/main
bin/main.o: src/main.cpp src/node.h src/decompress.h
	g++ $(CXXFLAGS) -c src/main.cpp -o bin/main.o
bin/decompress.o: src/decompress.cpp src/node.h src/int2byte.h
	g++ $(CXXFLAGS) -c src/decompress.cpp -o bin/decompress.o
bin/compress.o: src/compress.cpp src/compress.h src/node.h src/int2byte.h
	g++ $(CXXFLAGS) -c src/compress.cpp -o bin/compress.o
bin/parse.o: src/parse.cpp src/parse.h
	g++ $(CXXFLAGS) -c src/parse.cpp -o bin/parse.o
bin/node.o: src/node.cpp src/node.h
	g++ $(CXXFLAGS) -c src/node.cpp -o bin/node.o
clean:
	rm -f bin/main bin/*.o
