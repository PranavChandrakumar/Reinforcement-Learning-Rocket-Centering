# Compiler
CC = g++

# Executable
TARGET = ExecuteCentering

# Sources
SOURCES = $(wildcard *.cpp)

# Object Files
OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $^ -o $@

%.o: %.cpp
	$(CC) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJECTS)