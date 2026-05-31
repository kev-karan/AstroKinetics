CC = gcc
CFLAGS = -Wall -std=c99
SRC = main.c logic.c graphics.c utils.c

OBJ = $(SRC:.c=.o)

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Linux)
    LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
    TARGET = AstroKinetics.out
    RUN_CMD = ./$(TARGET)
else ifeq ($(UNAME_S), Darwin)
    LDFLAGS = -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
    TARGET = AstroKinetics.out
    RUN_CMD = ./$(TARGET)
else
    LDFLAGS = -lraylib -lopengl32 -lgdi32 -lwinmm
    TARGET = AstroKinetics.exe
    RUN_CMD = ./$(TARGET) 
endif

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(CFLAGS) $(LDFLAGS)

%.o: %.c game.h
	$(CC) -c $< -o $@ $(CFLAGS)

run: all
	$(RUN_CMD)

clean:
	rm -f $(OBJ) AstroKinetics.out AstroKinetics.exe