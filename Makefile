hw4: main.cpp robot.o log.o
	g++ -std=c++11 main.cpp robot.o log.o -lpthread -o hw4
robot.o: Robot.cpp Robot.h
	g++ -std=c++11 -c Robot.cpp -o robot.o
log.o: Log.cpp Log.h
	g++ -std=c++11 -c Log.cpp -o log.o

clean:
	rm *.o
	rm hw4
