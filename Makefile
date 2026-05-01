# Comp Vars
CXX = g++
CXXFLAGS_COMMON = -Wall -std=c++17

# Libraries
LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm -ldwmapi

# Includes
INCLUDES = -Iinclude -Isrc -Iraylib/include

# Library paths
LIBPATHS = -L. -Lraylib/lib

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
#     src/name.cpp \

# External source
SRC += include/json11.cpp

# Executable names
DEBUG_TARGET = bin/debug/funkin_debug.exe
RELEASE_TARGET = bin/release/funkin.exe

# Create folders if they don't exist
$(shell mkdir -p bin/release bin/test)

# Debug Mode
debug: CXXFLAGS = -O2 -Wall
debug: $(DEBUG_TARGET)
	@echo "Debug mode compiled! Run: ./funkin.exe"

# Release Mode
release: CXXFLAGS = $(CXXFLAGS_COMMON) -O2 -DNDEBUG -static-libgcc -static-libstdc++
release: LDFLAGS = -mwindows
release: $(RELEASE_TARGET)
	@echo "RELEASE mode compiled! Executable at: $(RELEASE_TARGET)"

# Debug build rule
$(DEBUG_TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(INCLUDES) $(LIBPATHS) $(LIBS)

# Release build rule
$(RELEASE_TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(INCLUDES) $(LIBPATHS) $(LIBS) $(LDFLAGS)

# Build everything
all: debug release
	@echo "Debug and Release builds complete!"

# Quick compile (without folders)
quick:
	$(CXX) $(CXXFLAGS_COMMON) -O2 -o funkin.exe $(SRC) $(INCLUDES) $(LIBPATHS) $(LIBS)

# Create package for friend in bin/test
package: release
	@echo "Creating package for friend in bin/test..."
	mkdir -p bin/test
	cp bin/release/funkin.exe bin/test/
	cp raylib/lib/raylib.dll bin/test/
	cp -r resources bin/test/
	@echo "Package created at bin/test/"

# Clean package
cleanpackage:
	rm -rf bin/test 2>nul
	@echo "Package cleaned!"

# Cleanup
clean:
	rm -f bin/debug/*.exe bin/release/*.exe 2>nul
	@echo "Cleanup complete!"

cleanall: clean
	rm -rf bin 2>nul
	@echo "Complete cleanup!"

.PHONY: debug release all clean cleanall quick package cleanpackage