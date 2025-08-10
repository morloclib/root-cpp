all:
	g++ -std=c++17 -g -Wall -O0 *.cpp *.hpp

.PHONY: clean
clean:
	rm -f *.o *.gch a.out
