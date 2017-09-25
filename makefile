#HEADERS = program.h headers.h
FLAGS = -Wall -Werror

default: all

obj/fsm_test.o: test/fsm_test.cpp $(HEADERS) obj/
	g++ $(FLAGS) -c test/fsm_test.cpp -o obj/fsm_test.o

all: obj/fsm_test.o
	g++ obj/fsm_test.o -o fsm_test

obj/:
	mkdir -p obj

clean:
	-rm -rf obj/*
	-rm -f fsm_test
