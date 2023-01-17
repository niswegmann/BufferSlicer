CC=clang
CFLAGS=-std=c++14 -I ..

BufferSlicer-Example: BufferSlicer-Example.cpp ../BufferSlicer.hpp
	$(CC) BufferSlicer-Example.cpp -o BufferSlicer-Example $(CFLAGS)

.PHONY: all

all: BufferSlicer-Example

.PHONY: clean

clean:

	rm BufferSlicer-Example
