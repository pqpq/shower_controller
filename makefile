#HEADERS = program.h headers.h
FLAGS = -std=c++11 -wall -werror

default: all

obj/fsm_test.o: test/fsm_test.cpp $(HEADERS)
	g++ $(FLAGS) -c test/fsm_test.cpp -o obj/fsm_test.o

all: obj/fsm_test.o
	g++ obj/fsm_test.o -o fsm_test

clean:
	-rm -rf obj/*
	-rm -f fsm_test