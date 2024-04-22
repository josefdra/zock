# Compiler, den du verwenden möchtest (z.B. g++, clang++)
CXX := g++
# Compiler Flags, z.B. -std=c++11 für C++11 Standardsupport
CXXFLAGS := -std=c++11 -Wall -Isrc/lib
# Linker Flags, hier leer, können aber zum Linken von Bibliotheken verwendet werden
LDFLAGS := 

# Quelldateien
SOURCES := $(wildcard src/src/*.cpp)
# Objektdateien, die aus den Quelldateien erstellt werden
OBJECTS := $(SOURCES:src/src/%.cpp=bin/%.o)
# Name des ausführbaren Programms
EXECUTABLE := bin/client01

# Das erste Ziel ist das Standardziel, das gebaut wird, wenn "make" ohne Argumente aufgerufen wird
all: $(EXECUTABLE)

# Linken der Objektdateien zum Erstellen des ausführbaren Programms
$(EXECUTABLE): $(OBJECTS)
	@mkdir -p bin
	$(CXX) $(LDFLAGS) $^ -o $@

# Kompilieren der Quelldateien in Objektdateien
bin/%.o: src/src/%.cpp
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Ein Ziel zum Bereinigen des Projekts (Entfernen aller gebauten Dateien)
clean:
	rm -f bin/.o*

.PHONY: all clean

