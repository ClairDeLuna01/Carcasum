CC=gcc
CFLAGS = -Wall -Werror
EXEC = carcassone

.PHONY: clean


main: main.c f_names.h
	$(CC) $(CFLAGS) $? -o $(EXEC)

debug: main.c f_names.h
	$(CC) $(CFLAGS) -g $? -o $(EXEC)

clean: 
	rm $(EXEC)
