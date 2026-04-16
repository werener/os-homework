SHELL = /bin/bash

CC = gcc

header_dir = include
target_dir = build
source_dir = src
data_dir = data

FLAGS = -Wall -Wextra -pedantic -I$(header_dir)

target = secure_copy
sources = queue.c caesar.c secure_copy.c main.c filepath.c logging.c
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

run: $(target_dir)/$(target) 
	./$(target_dir)/$(target) $(testset) ./$(data_dir)/out/ a

showcase: $(target_dir)/$(target)
	@echo -e "Running sequential mode:\n"
	./$(target_dir)/$(target) $(testset) -m=sequential ./$(data_dir)/out/ a
	@echo -e "Running parallel mode:\n"
	./$(target_dir)/$(target) $(testset) -m=parallel ./$(data_dir)/out/ a