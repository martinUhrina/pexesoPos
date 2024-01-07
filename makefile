CC=g++
CFLAGS=-std=c++14 -pthread

all: server client1 client2

server: main.cpp
	$(CC) $(CFLAGS) main.cpp -o server

client1: hrac1.cpp
	$(CC) $(CFLAGS) hrac1.cpp -o client1

client2: hrac2.cpp
	$(CC) $(CFLAGS) hrac2.cpp -o client2

clean:
	rm -f server client1 client2