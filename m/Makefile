# Comp Vars
CXX = g++
CXXFLAGS = -O2 -Wall

# Biblioteca
LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm -ldwmapi

# Includes
INCLUDES = -Iinclude -Isrc

# Files List
SRC = src/core/main.cpp \
      src/ui/editorUI.cpp \
      src/core/playstate.cpp \
      src/core/paths.cpp \
      src/characters/character.cpp \
      src/rhythm/conductor.cpp \
      src/stages/stage.cpp \
      src/ui/strumline.cpp \
      src/ui/health.cpp \
      src/debug/chartEditor.cpp \
      include/json11.cpp

TARGET = funkin.exe

all:
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC) $(INCLUDES) -L. $(LIBS)

clean:
	del $(TARGET)