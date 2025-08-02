# Compiler and Flags
CXX = g++
CXXFLAGS += -std=c++17 -O3 -Isrc \
  -Ilib/SDL2_build/include \
  -Ilib/SDL2_build/include/SDL2 \
  -Ilib/SDL2_image_build/include \
  -Ilib/SDL2_image_build/include/SDL2 \
  -Ilib/SDL2_ttf_build/include \
  -Ilib/SDL2_ttf_build/include/SDL2 \
  -Ilib/Lua_build/include

LDFLAGS += 	-Llib/SDL2_build/lib \
  -Llib/SDL2_image_build/lib \
  -Llib/SDL2_ttf_build/lib \
  -Llib/Lua_build/lib \
  -lSDL2 -lSDL2_image -lSDL2_ttf -llua

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
RPATH_BASE=@loader_path/../lib
else
RPATH_BASE=\$$ORIGIN/../lib
endif

ifeq ($(OS), Windows_NT)
	LDFLAGS += -lpsapi
else
  LDFLAGS += -Wl,-rpath,$(RPATH_BASE)/SDL2_build/lib \
    -Wl,-rpath,$(RPATH_BASE)/SDL2_image_build/lib \
    -Wl,-rpath,$(RPATH_BASE)/SDL2_ttf_build/lib \
    -Wl,-rpath,$(RPATH_BASE)/Lua_build/lib
endif

SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
CACHE_DIR = cache

RESOURCE_DIR = resources
SCRIPT_DIR = scripts

CONFIG_FILE = $(BIN_DIR)/config.ini

SOURCES = $(wildcard $(SRC_DIR)/**/*.cpp) $(wildcard $(SRC_DIR)/**/**/*.cpp) $(SRC_DIR)/main.cpp
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

TARGET = $(BIN_DIR)/project_doeville_x
DEBUG_TARGET = $(BIN_DIR)/project_doeville_x_debug
ASAN_TARGET = $(BIN_DIR)/project_doeville_x_asan
TSAN_TARGET = $(BIN_DIR)/project_doeville_x_tsan
PGO_GEN_TARGET = $(BIN_DIR)/project_doeville_x_pgo_gen
PGO_USE_TARGET = $(BIN_DIR)/project_doeville_x_pgo_use

all: deps $(TARGET) copy_config

debug: CXXFLAGS += -g -O0
ifeq ($(OS), Windows_NT)
debug: LDFLAGS += -mconsole
endif
debug: deps $(DEBUG_TARGET) copy_config

asan: CXXFLAGS += -g -O1 -fsanitize=address -fno-omit-frame-pointer
asan: LDFLAGS += -fsanitize=address
asan: deps $(ASAN_TARGET) copy_config

tsan: CXXFLAGS += -g -O1 -fsanitize=thread -fno-omit-frame-pointer
tsan: LDFLAGS += -fsanitize=thread
tsan: deps $(TSAN_TARGET) copy_config

pgo-generate: CXXFLAGS += -O3 -fprofile-generate
pgo-generate: LDFLAGS += -fprofile-generate
pgo-generate: deps $(PGO_GEN_TARGET) copy_config

pgo-use: CXXFLAGS += -O3 -fprofile-use
pgo-use: LDFLAGS += -fprofile-use
pgo-use: deps $(PGO_USE_TARGET) copy_config

$(TARGET): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Copying resources and scripts..."
	cp -r $(RESOURCE_DIR) $(BIN_DIR)/
	cp -r $(SCRIPT_DIR) $(BIN_DIR)/
	@mkdir -p $(BIN_DIR)/$(CACHE_DIR)

$(DEBUG_TARGET): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Copying resources and scripts..."
	cp -r $(RESOURCE_DIR) $(BIN_DIR)/
	cp -r $(SCRIPT_DIR) $(BIN_DIR)/
	@mkdir -p $(BIN_DIR)/$(CACHE_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(ASAN_TARGET): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Copying resources and scripts..."
	cp -r $(RESOURCE_DIR) $(BIN_DIR)/
	cp -r $(SCRIPT_DIR) $(BIN_DIR)/
	@mkdir -p $(BIN_DIR)/$(CACHE_DIR)

$(TSAN_TARGET): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Copying resources and scripts..."
	cp -r $(RESOURCE_DIR) $(BIN_DIR)/
	cp -r $(SCRIPT_DIR) $(BIN_DIR)/
	@mkdir -p $(BIN_DIR)/$(CACHE_DIR)

$(PGO_GEN_TARGET): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Copying resources and scripts..."
	cp -r $(RESOURCE_DIR) $(BIN_DIR)/
	cp -r $(SCRIPT_DIR) $(BIN_DIR)/
	@mkdir -p $(BIN_DIR)/$(CACHE_DIR)

$(PGO_USE_TARGET): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Copying resources and scripts..."
	cp -r $(RESOURCE_DIR) $(BIN_DIR)/
	cp -r $(SCRIPT_DIR) $(BIN_DIR)/
	@mkdir -p $(BIN_DIR)/$(CACHE_DIR)

copy_config:
	@echo "Copying config.ini to bin/"
	cp config.ini $(BIN_DIR)/

deps:
	@python3 package_check.py --fail-on-missing >/dev/null 2>&1 || \
	python3 package.py

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "Cleaned build directories."

.PHONY: all clean debug asan tsan pgo-generate pgo-use deps
