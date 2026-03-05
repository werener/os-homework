CC = gcc
FLAGS = -Wall -Wextra -pedantic
KEY = a

.PHONY: all clean

all: main

main: build/main

clean:
	rm -rf build/ \
	rm -f data/output*.txt

run: main
	./build/main ./data/input.txt ./data/output.txt a
	./build/main ./data/output.txt ./data/output2.txt a
	
build/main: src/main.c src/caesar.c src/queue.c src/secure_copy.c
	@mkdir -p $(@D)
	$(CC) $(FLAGS) $^ -o $@


