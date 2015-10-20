BOOST_PROTO=/opt/local/include
AD_LIB=/opt/local/lib
CCFLAGS=-I$(BOOST_PROTO) -L$(AD_LIB)
CXXFLAGS=-std=c++11 -stdlib=libc++

CC=g++

debug: CXXFLAGS += -O0 -ggdb
debug: all

default: all

all: nocdevice

clean:
	rm -f *.o

nocdevice: computer.o memory.o noc.o tree.o mux.o processor.o tile.o paging.o \
	functor.o router.o controlthread.o
	$(CC) $(CXXFLAGS) -o nocdevice $(CFLAGS) computer.o memory.o noc.o \
		tree.o mux.o processor.o tile.o paging.o \
		functor.o router.o controlthread.o

computer.o: computer.cpp memory.hpp tree.hpp noc.hpp tile.hpp \
	processor.hpp mux.hpp
	$(CC) $(CXXFLAGS)  -o computer.o -c -Wall computer.cpp

memory.o: memory.cpp memory.hpp tree.hpp
	$(CC) $(CXXFLAGS)  -o memory.o -c -Wall memory.cpp

noc.o: noc.cpp noc.hpp
	$(CC) $(CXXFLAGS)  -o noc.o -c -Wall noc.cpp

tree.o: tree.cpp tree.hpp
	$(CC) $(CXXFLAGS)  -o tree.o -c -Wall tree.cpp

mux.o: mux.hpp mux.cpp
	$(CC) $(CXXFLAGS)  -o mux.o -c -Wall mux.cpp

processor.o: processor.hpp processor.cpp
	$(CC) $(CXXFLAGS)  -o processor.o -c -Wall $(CCFLAGS) processor.cpp

tile.o: tile.hpp tile.cpp
	$(CC) $(CXXFLAGS)  -o tile.o -c -Wall tile.cpp

solver: gjsolver.o
	$(CC) $(CXXFLAGS) -o solver -Wall gjsolver.o $(CCFLAGS) -lgmpxx -lgmp

generator: lineargen.o
	$(CC) $(CXXFLAGS) -o generator -Wall lineargen.o

gjsolver.o: gjsolver.cpp
	$(CC) $(CXXFLAGS)  -o gjsolver.o -c -Wall $(CCFLAGS) gjsolver.cpp

lineargen.o: lineargen.cpp
	$(CC) $(CXXFLAGS)  -o lineargen.o -c -Wall lineargen.cpp

paging.o: paging.cpp paging.hpp
	$(CC) $(CXXFLAGS)  -o paging.o -c -Wall paging.cpp

functor.o: processorFunc.cpp
	$(CC) $(CXXFLAGS) -o functor.o -c -Wall processorFunc.cpp

router.o: router.cpp router.hpp
	$(CC) $(CXXFLAGS) -o router.o -c -Wall router.cpp

controlthread.o: ControlThread.cpp ControlThread.hpp
	$(CC) $(CXXFLAGS) -o controlthread.o -c -Wall ControlThread.cpp
	
