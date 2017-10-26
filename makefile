CC = gcc
CFLAGS = -Wall -std=c11 -g

CALENDARPARSERC = src/CalendarParser.c
CALENDARPARSERH = include/CalendarParser.h
CALENDARO = src/CalendarParser.o
LIBCPARSE = bin/libcparse.a

LINKEDLISTC = src/LinkedListAPI.c
LINKEDLISTH = include/LinkedListAPI.h
LISTO = src/LinkedListAPI.o
LIBLIST = bin/libllist.a

UIC = src/A2main.c
UIO = src/A2main.o

INCLUDES = include/
LIBS = -lcparse -lllist

UITARGET = bin/A2main

all:
	make list
	make parser
	make UI

list: $(LINKEDLISTC) $(LINKEDLISTH)
	$(CC) $(CFLAGS) -c $(LINKEDLISTC) -o $(LISTO) -I $(INCLUDES)
	ar cr $(LIBLIST) $(LISTO)

parser: $(LINKEDLISTC) $(LINKEDLISTH) $(CALENDARPARSERC) $(CALENDARPARSERH)
	$(CC) $(CFLAGS) -c $(CALENDARPARSERC) -o  $(CALENDARO) -I $(INCLUDES)
	ar cr $(LIBCPARSE) $(CALENDARO)

UI: $(UIC) $(CALENDARO) $(LISTO)
	$(CC) $(CFLAGS) $(UIC) -o $(UIO) -c -I $(INCLUDES)
	$(CC) $(CFLAGS) $(UIO) -Lbin/ $(LIBS) -o $(UITARGET)

clean:
	rm -f $(LIBLIST) $(LIBCPARSE) $(CALENDARO) $(LISTO) $(UIO) $(UITARGET)
