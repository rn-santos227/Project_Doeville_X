CXX = g++
CXXFLAGS += `sdl2-config --cflags` -I src -Ilib/SDL2/include -Ilib/SDL2_ttf/include -Ilib/Lua/include
LDFLAGS += `sdl2-config --libs` -Llib/SDL2/lib -Llib/SDL2_ttf/lib -Llib/Lua/lib -lSDL2 -lSDL2_ttf -llua

SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

SOURCES = $(wildcard $(SRC_DIR)/**/*.cpp) $(wildcard $(SRC_DIR)/**/**/*.cpp) $(SRC_DIR)/main.cpp
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
TARGET = $(BIN_DIR)/project_doeville_x

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)
	cp -r resources $(BIN_DIR)/resources

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
