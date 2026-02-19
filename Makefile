CC = g++
FLAGS = -Wall -Wextra -pedantic
KEY = a
lib:
	$(CC) $(FLAGS) -c -fPIC caesar.cpp -o caesar.o
	$(CC) $(FLAGS) -shared caesar.o -o libcaesar.so
main: 
	$(CC) $(FLAGS) -ldl main.cpp -o caesar
encode:
	make lib
	make main
	./caesar ./libcaesar.so $(KEY) input.txt output.txt
decode:
	./caesar ./libcaesar.so $(KEY) output.txt output2.txt

clean:
	rm -f caesar output*.txt *.o *.a *.so *.exe