hw1: main.cpp log.o
	g++ -std=c++11 main.cpp log.o -o hw1
log.o: Log.cpp Log.h
	g++ -std=c++11 -c Log.cpp -o log.o

clean:
	rm *.o
	rm hw1
