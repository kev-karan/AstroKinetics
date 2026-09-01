CC = gcc
CFLAGS = -Wall -std=c99 -I./src
SRC = src/main.c src/logic.c src/graphics.c src/utils.c

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


web:
	emcc $(SRC) -o index.html \
    -I./src -I./lib_web -L./lib_web -lraylib.web \
    --shell-file lib_web/minshell.html \
    -Os -Wall -DPLATFORM_WEB \
    -s USE_GLFW=3 -s ASYNCIFY \
	-s INITIAL_MEMORY=67108864 \
	-s ALLOW_MEMORY_GROWTH=1 \
    --preload-file assets