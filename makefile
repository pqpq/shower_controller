HEADERS = controller.h actions.h events.h beep.h
FLAGS = -std=c++11 -Wall -Werror

default: all

obj/fsm_test.o: test/fsm_test.cpp $(HEADERS) obj/
	g++ $(FLAGS) -c test/fsm_test.cpp -o obj/fsm_test.o

obj/beep_test.o: test/beep_test.cpp $(HEADERS) obj/
	g++ $(FLAGS) -c test/beep_test.cpp -o obj/beep_test.o

all: obj/fsm_test.o obj/beep_test.o
	g++ obj/fsm_test.o obj/beep_test.o -o fsm_test

obj/:
	mkdir -p obj

clean:
	-rm -rf obj/*
	-rm -f fsm_test
