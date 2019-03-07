hw3: main.cpp log.o
	g++ -std=c++11 main.cpp log.o -o hw3
log.o: Log.cpp Log.h
	g++ -std=c++11 -c Log.cpp -o log.o

clean:
	rm *.o
	rm hw3
