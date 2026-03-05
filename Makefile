CC = gcc
FLAGS = -Wall -Wextra -pedantic

target = secure_copy
target_dir = build

sources = queue.c caesar.c secure_copy.c main.c
objects = $(addprefix build/, $(sources:.c=.o))


.PHONY: all clean

main: $(target_dir)/$(target) 

clean:
	rm -rf build/ \
	rm -f data/output*.txt

$(target_dir)/$(target): $(objects)
	@mkdir -p $(@D)
	$(CC) $(FLAGS) $^ -o $@

build/%.o: src/%.c
	@mkdir -p $(@D)
	$(CC) $(FLAGS) -I src -c $< -o $@

run: $(target)
	./build/$(target) ./data/input.txt ./data/output.txt a
	./build/$(target) ./data/output.txt ./data/output2.txt a
	


