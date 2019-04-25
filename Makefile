all: hw5 server

server: server.cpp log.o
    g++ -std=c++11 server.cpp log.o -o server

hw5: main.cpp robot.o log.o
	g++ -std=c++11 main.cpp robot.o log.o -lpthread -o hw5
robot.o: Robot.cpp Robot.h
	g++ -std=c++11 -c Robot.cpp -o robot.o
log.o: Log.cpp Log.h
	g++ -std=c++11 -c Log.cpp -o log.o

clean:
	rm *.o
	rm hw5
	rm server