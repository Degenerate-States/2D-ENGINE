CC = g++
CFLAGS= -Wall -g $(INC)
LIBS = -lSDL2 -ldl
SRC = src
BUILD = build
ASSETS = assets

INC = -Iinclude -Ilib/GLAD/include -I/usr/include/SDL2 -Ilib/IMGUI
FILES = $(wildcard src/*.cpp)
PROGRAM_NAME = POLYGUN

# global defines
DEBUG_DEFINES = -DSOUND=false -DRENDER_SPINE=false -DCOLLISION_PRINT=false -DRENDER_GUI=true


all: debug

debug: $(FILES)
	@mkdir -p build
	$(CC) $(CFLAGS) $(DEBUG_DEFINES) $(FILES) lib/GLAD/src/glad.c lib/IMGUI/*.cpp -o $(BUILD)/$(PROGRAM_NAME) $(LIBS)


.PHONY: clean run
run:
	./$(BUILD)/$(PROGRAM_NAME)
clean:
	rm -rf $(BUILD)/*
