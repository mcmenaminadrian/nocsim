BOOST_PROTO=/opt/local/include
CCFLAGS=-I$(BOOST_PROTO)


default: all

all: nocdevice

clean:
	rm -f *.o

nocdevice: computer.o memory.o noc.o tree.o mux.o processor.o tile.o
	$(CC) -o nocdevice $(CFLAGS) computer.o memory.o noc.o \
		tree.o mux.o processor.o tile.o

computer.o: computer.cpp memory.hpp tree.hpp noc.hpp tile.hpp \
	processor.hpp mux.hpp
	g++ -O2 -o computer.o -c -Wall computer.cpp

memory.o: memory.cpp memory.hpp tree.hpp
	g++ -O2 -o memory.o -c -Wall memory.cpp

noc.o: noc.cpp noc.hpp
	g++ -O2 -o noc.o -c -Wall noc.cpp

tree.o: tree.cpp tree.hpp
	g++ -O2 -o tree.o -c -Wall tree.cpp

mux.o: mux.hpp mux.cpp
	g++ -O2 -o mux.o -c -Wall mux.cpp

processor.o: processor.hpp processor.cpp
	g++ -O2 -o processor.o -c -Wall $(CCFLAGS) processor.cpp

tile.o: tile.hpp tile.cpp
	g++ -O2 -o tile.o -c -Wall tile.cpp

solver: gjsolver.o
	g++ -o solver -Wall gjsolver.o -L/opt/local/lib -lgmpxx -lgmp

generator: lineargen.o
	g++ -o generator -Wall lineargen.o

gjsolver.o: gjsolver.cpp
	g++ -O2 -o gjsolver.o -c -Wall -I/opt/local/include gjsolver.cpp

lineargen.o: lineargen.cpp
	g++ -O2 -o lineargen.o -c -Wall lineargen.cpp
