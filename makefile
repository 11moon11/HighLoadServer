CC=c++
CXXFLAGS=-O2 -g -Wall -Wextra -MMD -std=c++11
LDLIBS=-lm -lpthread

main: main.o networking.o endpoint.o server.o client.o data.o

clean:
	$(RM) *.o *.d main

-include $(wildcard *.d)
