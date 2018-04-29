# makefile

all: dataserver client

NetworkRequestChannel.o: NetworkRequestChannel.h NetworkRequestChannel.cpp
	g++ -std=c++11 -c -g NetworkRequestChannel.cpp

bounded_buffer.o: bounded_buffer.h bounded_buffer.cpp semaphore.h
	g++ -std=c++11 -c -g bounded_buffer.cpp

dataserver: dataserver.cpp NetworkRequestChannel.o 
	g++ -std=c++11 -g -o dataserver dataserver.cpp NetworkRequestChannel.o -lpthread

client: client.cpp NetworkRequestChannel.o bounded_buffer.o semaphore.h
	g++ -std=c++11 -g -o client client.cpp NetworkRequestChannel.o bounded_buffer.o -lpthread

clean:
	rm -rf *.o fifo* dataserver client
