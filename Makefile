CC=gcc
LFLAGS=-lcurses
TARGET=conway

.PHONY: compile
compile: conway.c
	$(CC) $(LFLAGS) $^ -o $(TARGET)

clean:
	rm -f $(TARGET)
