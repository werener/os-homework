CXX = g++
CXXFLAGS = -Wall -std=c++11
AR = ar
ARFLAGS = rcs

libcaesar.a: caesar.o
	$(AR) $(ARFLAGS) $@ $^

caesar.o: caesar.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

main: maina.cpp caesar.cpp
clean:
	rm -f *.o *.a