CC = g++
CFLAGS= $(INC)
LIBS = -lSDL2 -ldl
SRC = src
BUILD = build
ASSETS = assets

INC = -Iinclude -Ilib/GLAD/include -I/usr/include/SDL2
FILES = $(wildcard src/*.cpp)
PROGRAM_NAME = POLYGUN

# global defines
DEBUG_DEFINES = -DSOUND=false -DRENDER_SPINE=ture -DCOLLISION_PRINT=false


all: debug

debug: $(FILES)
	$(CC) $(CFLAGS) $(DEBUG_DEFINES) $(FILES) lib/GLAD/src/glad.c -o $(BUILD)/$(PROGRAM_NAME) $(LIBS)


.PHONY: clean run
run:
	./$(BUILD)/$(PROGRAM_NAME)
clean:
	rm -f $(BUILD)/*
