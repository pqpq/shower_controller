#HEADERS = program.h headers.h

default: all

fsm_test.o: fsm_test.cpp $(HEADERS)
	gcc -c fsm_test.cpp -o fsm_test.o

all: fsm_test.o
	gcc fsm_test.o -o fsm_test

clean:
	-rm -f fsm_test.o
	-rm -f fsm_test