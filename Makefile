CC=gcc
CFLAGS = -Wall -Werror -lm
EXEC = carcassone

.PHONY: clean

debug: main.c 
	$(CC) $(CFLAGS) -g $? f_names.h -o $(EXEC)

install: main.c
	$(CC) $(CFLAGS) -O2 $? f_names.h -o $(EXEC)

clean: 
	rm $(EXEC)
