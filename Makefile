# ====================
# Cross-Platform Setup
# ====================

ifeq ($(OS),Windows_NT)
  NUL = NUL
  PYTHON = python
  MKDIR = mkdir
  COPY = xcopy /E /I /Y
  RM = rmdir /S /Q
  SLASH = \\
  SEP = ;
  RPATH_FLAG =
else
  NUL = /dev/null
  PYTHON = python3
  MKDIR = mkdir -p
  COPY = cp -r
  RM = rm -rf
  SLASH = /
  SEP = :
  UNAME_S := $(shell uname -s)
  ifeq ($(UNAME_S),Darwin)
    RPATH_BASE = @loader_path/../lib
  else
    RPATH_BASE = \$$ORIGIN/../lib
  endif
  RPATH_FLAG = -Wl,-rpath,$(RPATH_BASE)/SDL2_build/lib \
               -Wl,-rpath,$(RPATH_BASE)/SDL2_image_build/lib \
               -Wl,-rpath,$(RPATH_BASE)/SDL2_ttf_build/lib \
               -Wl,-rpath,$(RPATH_BASE)/Lua_build/lib
endif

# ====================
# Compiler and Flags
# ====================

CXX = g++
CXXFLAGS += -std=c++17 -O3 -Isrc \
  -Ilib/SDL2_build/include \
  -Ilib/SDL2_build/include/SDL2 \
  -Ilib/SDL2_image_build/include \
  -Ilib/SDL2_image_build/include/SDL2 \
  -Ilib/SDL2_ttf_build/include \
  -Ilib/SDL2_ttf_build/include/SDL2 \
  -Ilib/Lua_build/include

LDFLAGS += -Llib/SDL2_build/lib \
  -Llib/SDL2_image_build/lib \
  -Llib/SDL2_ttf_build/lib \
  -Llib/Lua_build/lib \
  -lSDL2 -lSDL2_image -lSDL2_ttf -llua -lz \
  $(RPATH_FLAG)

ifeq ($(OS),Windows_NT)
  LDFLAGS += -lpsapi
endif

# ====================
# Directories & Files
# ====================

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

# ====================
# Main Targets
# ====================

all: deps $(TARGET) copy_config

debug: CXXFLAGS += -g -O0
ifeq ($(OS),Windows_NT)
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

# ====================
# Build Rules
# ====================

$(TARGET): $(OBJECTS)
	@$(MKDIR) $(BIN_DIR)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Copying resources and scripts..."
	$(COPY) $(RESOURCE_DIR) $(BIN_DIR)
	$(COPY) $(SCRIPT_DIR) $(BIN_DIR)
	@$(MKDIR) $(BIN_DIR)/$(CACHE_DIR)

$(DEBUG_TARGET): $(OBJECTS)
	@$(MKDIR) $(BIN_DIR)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Copying resources and scripts..."
	$(COPY) $(RESOURCE_DIR) $(BIN_DIR)
	$(COPY) $(SCRIPT_DIR) $(BIN_DIR)
	@$(MKDIR) $(BIN_DIR)/$(CACHE_DIR)

$(ASAN_TARGET): $(OBJECTS)
	@$(MKDIR) $(BIN_DIR)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Copying resources and scripts..."
	$(COPY) $(RESOURCE_DIR) $(BIN_DIR)
	$(COPY) $(SCRIPT_DIR) $(BIN_DIR)
	@$(MKDIR) $(BIN_DIR)/$(CACHE_DIR)

$(TSAN_TARGET): $(OBJECTS)
	@$(MKDIR) $(BIN_DIR)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Copying resources and scripts..."
	$(COPY) $(RESOURCE_DIR) $(BIN_DIR)
	$(COPY) $(SCRIPT_DIR) $(BIN_DIR)
	@$(MKDIR) $(BIN_DIR)/$(CACHE_DIR)

$(PGO_GEN_TARGET): $(OBJECTS)
	@$(MKDIR) $(BIN_DIR)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Copying resources and scripts..."
	$(COPY) $(RESOURCE_DIR) $(BIN_DIR)
	$(COPY) $(SCRIPT_DIR) $(BIN_DIR)
	@$(MKDIR) $(BIN_DIR)/$(CACHE_DIR)

$(PGO_USE_TARGET): $(OBJECTS)
	@$(MKDIR) $(BIN_DIR)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Copying resources and scripts..."
	$(COPY) $(RESOURCE_DIR) $(BIN_DIR)
	$(COPY) $(SCRIPT_DIR) $(BIN_DIR)
	@$(MKDIR) $(BIN_DIR)/$(CACHE_DIR)

# ====================
# Object Compilation
# ====================

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@$(MKDIR) $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ====================
# Helper Targets
# ====================

copy_config:
	@echo "Copying config.ini to bin/"
	$(COPY) config.ini $(BIN_DIR)

deps:
	@$(PYTHON) package_check.py --fail-on-missing >$(NUL) 2>&1 || \
	$(PYTHON) package.py

clean:
	$(RM) $(BUILD_DIR)
	$(RM) $(BIN_DIR)
	@echo "Cleaned build directories."

.PHONY: all clean debug asan tsan pgo-generate pgo-use deps copy_config
