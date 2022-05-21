CC=gcc
CFLAGS = -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer 

EXEC = Carcasum
WINFLAGS = -lmingw32

.PHONY: clean

default: source/carcasum.c source/carcasum.h
	$(CC) -g $? -o $(EXEC) -lm -Wall $(CFLAGS) 

	
install: source/carcasum.c source/carcasum.h
	$(CC) -g -O3 $? -o $(EXEC) -lm $(CFLAGS) 

windows: source/carcasum.c source/carcasum.h
	$(CC) -std=c17 -g $? icon.res -o $(EXEC).exe -lm -Wall $(WINFLAGS) $(CFLAGS) 

install-win: source/carcasum.c source/carcasum.h
	$(CC) -std=c17 -O3 $? icon.res -o $(EXEC).exe -lm $(WINFLAGS) $(CFLAGS) 

run:
	./$(EXEC)

clean: 
	rm $(EXEC)
