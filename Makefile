CC=gcc
CFLAGS = -Wall -Werror
EXEC = carcassone

.PHONY: clean

debug: main.c 
	$(CC) $(CFLAGS) -g $? f_names.h -lm -o $(EXEC)

install: main.c
	$(CC) $(CFLAGS) -O2 $? f_names.h -lm -o $(EXEC)

clean: 
	rm $(EXEC)
