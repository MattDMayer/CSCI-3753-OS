# CSCI3753 - Spring 2023 PA4

CC = gcc
CFLAGS = -c -g -Wall -Wextra
LFLAGS = -g -Wall -Wextra

SUBMITFILES = pager-lru.c pager-predict.c

.PHONY: all clean

all: test-basic test-lru pager-predict.c

test-basic: simulator.o pager-basic.o programs.o
	$(CC) $(LFLAGS) $^ -o $@

test-lru: simulator.o pager-lru.o programs.o
	$(CC) $(LFLAGS) $^ -o $@

test-predict: simulator.o pager-predict.o programs.o
	$(CC) $(LFLAGS) $^ -o $@

simulator.o: simulator.c programs.o simulator.h
	$(CC) $(CFLAGS) $<

programs.o: programs.c simulator.h
	$(CC) $(CFLAGS) $<

pager-basic.o: pager-basic.c simulator.h programs.c
	$(CC) $(CFLAGS) $<

pager-lru.o: pager-lru.c simulator.h programs.c   
	$(CC) $(CFLAGS) $<

pager-predict.o: pager-predict.c simulator.h
	$(CC) $(CFLAGS) $<

clean:
	rm -f test-basic test-lru test-predict test-api
	rm -f *.o
	rm -f *~
	rm -f *.csv
	rm -f *.pdf

submit: 
	@read -r -p "please enter your identikey username: " username; \
	tar -cvf PA4-$$username.txt $(SUBMITFILES)
