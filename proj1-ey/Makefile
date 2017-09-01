	CC = gcc
	CFLAGS = -g -Wall -c
	LDFLAGS = -g -Wall

all: philspel

philspel : philspel.o hashtable.o
	$(CC) $(LDFLAGS) -o philspel philspel.o hashtable.o

philspel.o : philspel.c philspel.h hashtable.h
	$(CC) $(CFLAGS) philspel.c

hashtable.o : hashtable.c hashtable.h
	$(CC) $(CFLAGS) hashtable.c

clean :
	rm *.o

test : clean philspel
	touch testOutput
	cat sampleInput | ./philspel sampleDictionary >	 testOutput
	@echo The following should be empty if there are no problems
	diff sampleOutput testOutput 2> /dev/null
	@echo Testing complete

