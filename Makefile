# Compiler and Flags
CXX = g++
CXXFLAGS += -O2 -Isrc \
            -Ilib/SDL2/SDL2-2.26.5/include \
			-Ilib/SDL2_image/SDL2_image-2.6.3 \
            -Ilib/SDL2_ttf/SDL2_ttf-2.20.1 \
            -Ilib/Lua/lua-5.4.6/src

LDFLAGS += `sdl2-config --libs` \
           -Llib/SDL2/SDL2-2.26.5/lib \
		   -Llib/SDL2_image/SDL2_image-2.6.3 \
           -Llib/SDL2_ttf/SDL2_ttf-2.20.1 \
           -Llib/Lua/lua-5.4.6/src \
           -lSDL2 -lSDL2_image -lSDL2_ttf -llua

ifeq ($(OS),Windows_NT)
	LDFLAGS += -lpsapi
endif

SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

RESOURCE_DIR = resources
SCRIPT_DIR = scripts

CONFIG_FILE = $(BIN_DIR)/config.ini

SOURCES = $(wildcard $(SRC_DIR)/**/*.cpp) $(wildcard $(SRC_DIR)/**/**/*.cpp) $(SRC_DIR)/main.cpp
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

TARGET = $(BIN_DIR)/project_doeville_x
DEBUG_TARGET = $(BIN_DIR)/project_doeville_x_debug

all: $(TARGET) copy_config

debug: CXXFLAGS += -g -O0
debug: LDFLAGS += -mconsole
debug: $(DEBUG_TARGET) copy_config

$(TARGET): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Copying resources and scripts..."
	cp -r $(RESOURCE_DIR) $(BIN_DIR)/
	cp -r $(SCRIPT_DIR) $(BIN_DIR)/

$(DEBUG_TARGET): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Copying resources and scripts..."
	cp -r $(RESOURCE_DIR) $(BIN_DIR)/
	cp -r $(SCRIPT_DIR) $(BIN_DIR)/

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

copy_config:
	@echo "Copying config.ini to bin/"
	cp config.ini $(BIN_DIR)/

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "Cleaned build directories."

.PHONY: all clean debug
