CC=gcc -g -pthread
all: clean server client
server: server.o
clean:
		rm -f *.o server utility client
rebuild: clean all