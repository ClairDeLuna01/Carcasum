CC=gcc
CFLAGS = -Wall  -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer 

EXEC = Carcasum
WINFLAGS = -lmingw32

.PHONY: clean

default: source/carcasum.c
	$(CC) -g $? -o $(EXEC) -lm $(CFLAGS) 

	
install: source/carcasum.c
	$(CC) -O3 $? -o $(EXEC) -lm $(CFLAGS) 

windows: source/carcasum.c
	$(CC) -std=c17 -g $? icon.res -o $(EXEC).exe -lm $(WINFLAGS) $(CFLAGS) 

install-win: source/carcasum.c
	$(CC) -std=c17 -O3 $? icon.res -o $(EXEC).exe -lm $(WINFLAGS) $(CFLAGS) 

run:
	./$(EXEC)

clean: 
	rm $(EXEC)
