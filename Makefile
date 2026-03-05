CC = gcc
FLAGS = -Wall -Wextra -pedantic
KEY = a

.PHONY: all clean

all: main

main: build/main

clean:
	rm -rf build/ \
	rm -f data/output*.txt

build/main: src/main.c src/caesar.c src/queue.c
	@mkdir -p $(@D)
	$(CC) $(FLAGS) $^ -o $@


