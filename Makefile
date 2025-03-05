CXX = g++
CXXFLAGS = -std=c++11 -Wall -I./src
SRC = src/main.cpp
OBJ = $(SRC:src/%.cpp=bin/%.o)
TARGET = bin/FM_WKK

# Ensure the bin directory exists
BIN_DIR = bin
$(shell mkdir -p $(BIN_DIR))

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^

bin/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(BIN_DIR)/*.o $(TARGET)
