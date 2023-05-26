SRC_DIR = src
BUILD_DIR = build/debug
RELEASE_DIR = build/release
CC = clang
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_NAME = Bees
INCLUDE_PATHS = -I libs/sdl2/include/SDL2/  -I libs/SDL2_TTF/include/ -I libs/StringView/sv.h
LIBRARY_PATHS = -L libs/sdl2/lib/ -L libs/SDL2_TTF/lib/
LINKER_FLAGS = -lSDL2 -lSDL2_TTF


all:
	$(CC) $(COMPILER_FLAGS) $(LINKER_FLAGS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(SRC_FILES)  -o $(BUILD_DIR)/$(OBJ_NAME)
clean:
	rm -rf $(BUILD_DIR)/*
	rm -rf $(RELEASE_DIR)/*

cleanbuild:
	rm -rf $(BUILD_DIR)/*
	rm -rf $(RELEASE_DIR)/*
	$(CC) $(COMPILER_FLAGS) $(LINKER_FLAGS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(SRC_FILES)  -o $(BUILD_DIR)/$(OBJ_NAME)