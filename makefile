CC = gcc
CFLAGS = -Wall -std=c11 -g
MAINC = src/Main.c
MAINO = src/Main.o

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

TARGET = iCalendar
UITARGET = UI

all:
	make list
	make parser
	make main
	make UI

run:
	./$(TARGET)

run-ui:
	./$(UITARGET)

list: $(LINKEDLISTC) $(LINKEDLISTH)
	$(CC) $(CFLAGS) -c $(LINKEDLISTC) -o $(LISTO) -I $(INCLUDES)
	ar cr $(LIBLIST) $(LISTO)

parser: $(LINKEDLISTC) $(LINKEDLISTH) $(CALENDARPARSERC) $(CALENDARPARSERH)
	$(CC) $(CFLAGS) -c $(CALENDARPARSERC) -o  $(CALENDARO) -I $(INCLUDES)
	ar cr $(LIBCPARSE) $(CALENDARO)

main: $(MAINC)
	$(CC) $(CFLAGS) $(MAINC) -o $(MAINO) -c -I $(INCLUDES)
	$(CC) $(CFLAGS) $(MAINO) -Lbin/ $(LIBS) -o $(TARGET)

UI: $(UIC) $(CALENDARO) $(LISTO)
	$(CC) $(CFLAGS) $(UIC) -o $(UIO) -c -I $(INCLUDES)
	$(CC) $(CFLAGS) $(UIO) -Lbin/ $(LIBS) -o $(UITARGET)


valgrind:
	valgrind --leak-check=full ./$(TARGET)

clean:
	rm -f $(LIBLIST) $(LIBCPARSE) $(CALENDARO) $(LISTO) $(MAINO) $(TARGET) $(UITARGET)
