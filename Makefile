CC = gcc
CFLAGS = -Wall -std=c99
LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

SRC = main.c logic.c graphics.c utils.c

OBJ = $(SRC:.c=.o)

TARGET = AstroKinetics.out

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(CFLAGS) $(LDFLAGS)

%.o: %.c game.h
	$(CC) -c $< -o $@ $(CFLAGS)

run: all
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)