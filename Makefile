SHELL = /bin/bash

CC = gcc

header_dir = include
lib_dir = lib
target_dir = build
source_dir = src
data_dir = data

FLAGS = -Wall -Wextra -pedantic -I$(header_dir) -I$(lib_dir)

target = secure_copy
sources = queue.c caesar.c secure_copy.c main.c filepath.c logging.c args.c image.c image_operations.c
objects = $(addprefix $(target_dir)/, $(sources:.c=.o))
	
testset = $(shell echo ./$(data_dir)/f{1..8}.txt)

.PHONY: main clean run

main: $(target_dir)/$(target) 

clean:
	rm -rf $(target_dir)/
	rm -rf $(data_dir)/out
	@mkdir -p $(data_dir)/out
	rm -f log.log

$(target_dir)/$(target): $(objects)
	@mkdir -p $(@D)
	$(CC) $(FLAGS) $^ -o $@

$(target_dir)/%.o: $(source_dir)/%.c
	@mkdir -p $(@D)
	$(CC) $(FLAGS) -I$(header_dir) -c $< -o $@