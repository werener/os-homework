CC = g++
FLAGS = -Wall -Wextra -pedantic
lib:
	$(CC) $(FLAGS) -c -fPIC caesar.cpp -o caesar.o
	$(CC) $(FLAGS) -shared caesar.o -o libcaesar.so
main: 
	$(CC) $(FLAGS) -ldl main.cpp -o caesar

clean:
	rm -f main *.o *.a *.so *.exe