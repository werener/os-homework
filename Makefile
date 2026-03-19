CC = gcc

header_dir = include
target_dir = build
source_dir = src
data_dir = data

FLAGS = -Wall -Wextra -pedantic -I$(header_dir)

target = secure_copy
sources = queue.c caesar.c secure_copy.c main.c filepath.c
objects = $(addprefix $(target_dir)/, $(sources:.c=.o))

.PHONY: main clean run

main: $(target_dir)/$(target) 

clean:
	rm -rf $(target_dir)/
	rm -f $(data_dir)/output*.txt

$(target_dir)/$(target): $(objects)
	@mkdir -p $(@D)
	$(CC) $(FLAGS) $^ -o $@

$(target_dir)/%.o: $(source_dir)/%.c
	@mkdir -p $(@D)
	$(CC) $(FLAGS) -I$(header_dir) -c $< -o $@

run: $(target_dir)/$(target) 
	./$(target_dir)/$(target) ./$(data_dir)/f1.txt ./$(data_dir)/out a
	./$(target_dir)/$(target) ./$(data_dir)/output.txt ./$(data_dir)/out a

test: run
	@echo "Difference of input and output: \n" 
	@diff data/input.txt data/output2.txt