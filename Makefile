CC = gcc
CFLAGS = -Wall -std=c99
LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

all:
	$(CC) main.c -o AstroKinetics.out $(CFLAGS) $(LDFLAGS)
run: all
	./AstroKinetics.out
clean:
	rm -f AstroKinetics.out