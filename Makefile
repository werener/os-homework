CXX = g++
FLAGS = -Wall -Wextra -pedantic
lib:
	$(CXX) $(FLAGS) -fPIC -c caesar.cpp -o caesar.o 
	$(CXX) $(FLAGS) -shared -o libcaesar.so caesar.o

main: 
	$(CXX) $(FLAGS) -o main.exe main.cpp -L. -lcaesar

all: 
	$(CXX) $(FLAGS) -fPIC -c caesar.cpp -o caesar.o 
	$(CXX) $(FLAGS) -shared -o libcaesar.so caesar.o 
	$(CXX) $(FLAGS) -o main.exe main.cpp -L. -lcaesar
run: 
	$(CXX) $(FLAGS) -fPIC -c caesar.cpp -o caesar.o 
	$(CXX) $(FLAGS) -shared -o libcaesar.so caesar.o 
	$(CXX) $(FLAGS) -o main.exe main.cpp -L. -lcaesar 
	./main.exe
test: 
	$(CXX) $(FLAGS) caesar.cpp test.cpp -o test.exe 
	./test.exe
clean:
	rm -f main *.o *.a *.so *.exe