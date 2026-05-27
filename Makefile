SHELL = /bin/bash

CC = gcc

header_dir = include
lib_dir = lib
target_dir = build
source_dir = src
data_dir = data

FLAGS = -Wall -Wextra -pedantic -I$(header_dir) -I$(lib_dir)

target = secure_copy
libs = queue.c array.c
sources = $(libs) main.c filepath.c logging.c args.c image.c image_operations.c rc4.c
objects = $(addprefix $(target_dir)/, $(sources:.c=.o))
	
testset = $(shell echo ./$(data_dir)/f{1..8}.txt)

.PHONY: main clean run

main: $(target_dir)/$(target) 

clean:
	rm -rf $(target_dir)/
	rm -rf $(data_dir)/out
	@mkdir -p $(data_dir)/out
	rm -f log.log

# Binary compilation
$(target_dir)/$(target): $(objects)
	@mkdir -p $(@D)
	@$(CC) $(FLAGS) $^ -o $@
	@echo -e "\nFinished!"

# Object compilation
$(target_dir)/%.o: $(lib_dir)/%.c
	@mkdir -p $(@D)
	@echo -n "$@+  "
	@$(CC) $(FLAGS) -I$(header_dir) -c $< -o $@

$(target_dir)/%.o: $(source_dir)/%.c
	@mkdir -p $(@D)
	@echo -n "$@+  "
	@$(CC) $(FLAGS) -I$(header_dir) -c $< -o $@