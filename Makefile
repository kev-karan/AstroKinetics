CC = gcc
CFLAGS = -Wall -std=c99
LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

all:
	$(CC) main.c -o AstroKinetics $(CFLAGS) $(LDFLAGS)
run: all
	./AstroKinetics
clean:
	rm -f AstroKinetics