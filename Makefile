# |================================|
# |  Funkin C++          Makefile  |
# |================================|

CXX = g++
CXXFLAGS_COMMON := -Wall -std=c++17

LIBS := -lraylib
ifeq ($(OS),Windows_NT)
	LIBS +=-lopengl32 -lgdi32 -lwinmm -ldwmapi
endif

LIBPATHS := -L. -Lraylib/lib
INCLUDES := -Iinclude -Isrc -Iraylib/include
SRC := src/core/main.cpp \
      src/ui/editorUI.cpp \
      src/core/playstate.cpp \
      src/core/paths.cpp \
      src/characters/character.cpp \
      src/rhythm/conductor.cpp \
      src/stages/stage.cpp \
      src/ui/strumline.cpp \
      src/ui/health.cpp \
      src/debug/chartEditor.cpp

SRC += include/json11.cpp

RES_DIR = ./resources

BIN_DIR = ./bin
BIN_FILE = funkin
ifeq ($(OS),Windows_NT)
	BIN_FILE := $(BIN_FILE).exe
endif

DEBUG_BIN_DIR := $(BIN_DIR)/debug
RELEASE_BIN_DIR := $(BIN_DIR)/release

DEBUG_TARGET := $(DEBUG_BIN_DIR)/$(BIN_FILE)
RELEASE_TARGET := $(RELEASE_BIN_DIR)/$(BIN_FILE)

all: build

setup-bin:
	mkdir -p $(BIN_DIR)
	cp raylib/lib/raylib.dll $(BIN_DIR)
	cp -r $(RES_DIR) $(BIN_DIR)

$(DEBUG_TARGET): $(SRC)
	$(CXX) $(CXXFLAGS_COMMON) $(CXXFLAGS) $^ -o $@ -O2 -g $(INCLUDES) $(LIBS) $(LIBPATHS) $(LDFLAGS)

$(RELEASE_TARGET): $(SRC)
	$(CXX) $(CXXFLAGS_COMMON) $(CXXFLAGS) $^ -o $@ -O3 $(INCLUDES) $(LIBS) $(LIBPATHS) $(LDFLAGS)

# quick: $(SRC)
# 	$(CXX) $(CXXFLAGS_COMMON) $^ -o ./$(BIN_FILE) -O1 $(INCLUDES) $(LIBS) $(LIBPATHS)

build: BIN_DIR = $(DEBUG_BIN_DIR)
build: setup-bin $(DEBUG_TARGET)

ifeq ($(OS),Windows_NT)
	build-release: LDFLAGS = -mwindows
endif
build-release: BIN_DIR = $(RELEASE_BIN_DIR)
build-release: setup-bin $(RELEASE_TARGET)

run: build
	$(DEBUG_TARGET)

run-release: build-release
	$(RELEASE_TARGET)

clean:
	rm -rf $(BIN_DIR)

.PHONY: setup-bin build build-release run run-release clean
