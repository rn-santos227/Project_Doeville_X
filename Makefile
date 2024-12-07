CXX = g++
CXXFLAGS = -std=c++17 -Wall -I./src

SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

SOURCES = $(SRC_DIR)/main.cpp
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

TARGET = $(BIN_DIR)/$(notdir $(CURDIR))

CXXFLAGS += -mconsole

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(OBJECTS) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
