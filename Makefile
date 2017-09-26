CC=g++

CFLAGS=-c -Wall
CINCDIR=-I 3rd-party
CLIBDIR=-L 3rd-party

all: minidbg

minidbg: main.o debugger.o
	$(CC) $(CLIBDIR)/linenoise main.o debugger.o -llinenoise -o minidbg

main.o: main.cpp
	$(CC) $(CFLAGS) $(CINCDIR) main.cpp

debugger.o: debugger.cpp debugger.hpp
	$(CC) $(CFLAGS) $(CINCDIR) debugger.cpp

clean:
	rm -rf *.o minidbg