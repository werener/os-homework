CC = gcc
FLAGS = -Wall -Wextra -pedantic
KEY = a

.PHONY: all lib main clean

all: lib main

lib: build/libcaesar.so

main: build/main
	
clean:
	rm -rf build/ \
	rm -f data/output*.txt

build/libcaesar.so: build/caesar.o
	@mkdir -p $(@D)
	$(CC) $(FLAGS) -shared $< -o $@
	rm -f $<

build/caesar.o: caesar.c caesar.h
	@mkdir -p $(@D)
	$(CC) $(FLAGS) -c -fPIC caesar.c -o $@

build/main: main.c
	@mkdir -p $(@D)
	$(CC) $(FLAGS) -ldl $< -o $@

encode:
	./build/main ./build/libcaesar.so $(KEY) \
	./data/input.txt ./data/output.txt

decode:
	./build/main ./build/libcaesar.so $(KEY) \
	./data/output.txt ./data/output2.txt

