CC = gcc

header_dir = include
target_dir = build
source_dir = src
data_dir = data

FLAGS = -Wall -Wextra -pedantic -I$(header_dir)

target = secure_copy
sources = queue.c caesar.c secure_copy.c main.c filepath.c logging.c
objects = $(addprefix $(target_dir)/, $(sources:.c=.o))

files = f1 f2 f3 f4
testset = $(addprefix ./$(data_dir)/,$(addsuffix .txt,$(files)))

.PHONY: main clean run

main: $(target_dir)/$(target) 

clean:
	rm -rf $(target_dir)/
	rm -rf $(data_dir)/out
	@mkdir -p $(data_dir)/out
	rm -f log.txt

$(target_dir)/$(target): $(objects)
	@mkdir -p $(@D)
	$(CC) $(FLAGS) $^ -o $@

$(target_dir)/%.o: $(source_dir)/%.c
	@mkdir -p $(@D)
	$(CC) $(FLAGS) -I$(header_dir) -c $< -o $@

run: $(target_dir)/$(target) 
	./$(target_dir)/$(target) $(testset) ./$(data_dir)/out/ a
