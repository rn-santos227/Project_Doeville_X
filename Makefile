.DEFAULT_GOAL := all

CXX ?= g++
ifeq ($(OS),Windows_NT)
PYTHON ?= python
EXEEXT := .exe
else
PYTHON ?= python3
EXEEXT :=
endif

MODE ?= release
MODES := release debug asan tsan pgo-generate pgo-use
ifeq ($(filter $(MODE),$(MODES)),)
$(error Unsupported build mode: $(MODE))
endif

SRC_DIR := src
BUILD_DIR := build
BIN_DIR := bin
OBJECT_DIR := $(BUILD_DIR)/$(MODE)
PROFILE_DIR := $(abspath cache/pgo)
ifneq ($(filter $(MODE),pgo-generate pgo-use),)
PROFILE_PREFIX := $(shell $(PYTHON) -c "import os; print(os.path.join(os.getcwd(), '$(OBJECT_DIR)'))")
endif

CPPFLAGS += -Isrc \
  -Ilib/SDL2_build/include \
  -Ilib/SDL2_build/include/SDL2 \
  -Ilib/SDL2_image_build/include \
  -Ilib/SDL2_image_build/include/SDL2 \
  -Ilib/SDL2_ttf_build/include \
  -Ilib/SDL2_ttf_build/include/SDL2 \
  -Ilib/Lua_build/include

MODE_FLAGS_release := -O3
MODE_FLAGS_debug := -g -O0
MODE_FLAGS_asan := -g -O1 -fsanitize=address -fno-omit-frame-pointer
MODE_FLAGS_tsan := -g -O1 -fsanitize=thread -fno-omit-frame-pointer
MODE_FLAGS_pgo-generate := -O3 -fprofile-generate="$(PROFILE_DIR)" -fprofile-prefix-path="$(PROFILE_PREFIX)"
MODE_FLAGS_pgo-use := -O3 -fprofile-use="$(PROFILE_DIR)" -fprofile-prefix-path="$(PROFILE_PREFIX)"
CXXFLAGS += -std=c++17 $(MODE_FLAGS_$(MODE))

MODE_LDFLAGS_asan := -fsanitize=address
MODE_LDFLAGS_tsan := -fsanitize=thread
MODE_LDFLAGS_pgo-generate := -fprofile-generate="$(PROFILE_DIR)"
MODE_LDFLAGS_pgo-use := -fprofile-use="$(PROFILE_DIR)"
LDFLAGS += -Llib/SDL2_build/lib \
  -Llib/SDL2_image_build/lib \
  -Llib/SDL2_ttf_build/lib \
  -Llib/Lua_build/lib \
  $(MODE_LDFLAGS_$(MODE))
LDLIBS += -lSDL2 -lSDL2_image -lSDL2_ttf -llua -lz

ifeq ($(OS),Windows_NT)
LDLIBS += -lpsapi
MODE_LDFLAGS_debug := -mconsole
else
PLATFORM := $(shell $(PYTHON) -c "import sys; print(sys.platform)")
ifeq ($(PLATFORM),darwin)
RPATH_BASE := @loader_path/../lib
else
RPATH_BASE := \$$ORIGIN/../lib
endif
LDFLAGS += -Wl,-rpath,$(RPATH_BASE)/SDL2_build/lib \
  -Wl,-rpath,$(RPATH_BASE)/SDL2_image_build/lib \
  -Wl,-rpath,$(RPATH_BASE)/SDL2_ttf_build/lib \
  -Wl,-rpath,$(RPATH_BASE)/Lua_build/lib
endif

SOURCES := $(shell $(PYTHON) -B tools/build_support.py sources)
OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJECT_DIR)/%.o,$(SOURCES))
SUFFIX_release :=
SUFFIX_debug := _debug
SUFFIX_asan := _asan
SUFFIX_tsan := _tsan
SUFFIX_pgo-generate := _pgo_gen
SUFFIX_pgo-use := _pgo_use
TARGET := $(BIN_DIR)/project_doeville_x$(SUFFIX_$(MODE))$(EXEEXT)
PHYSICS_TEST_OBJECT := $(OBJECT_DIR)/tests/physics_tests.o
PHYSICS_TEST_TARGET := $(BIN_DIR)/physics_tests$(SUFFIX_$(MODE))$(EXEEXT)
ENGINE_OBJECTS := $(filter-out $(OBJECT_DIR)/main.o,$(OBJECTS))

all: $(TARGET)

release debug asan tsan pgo-generate pgo-use:
	$(MAKE) MODE=$@ all

$(TARGET): $(OBJECTS) | copy_runtime
	$(CXX) $(OBJECTS) -o "$@" $(LDFLAGS) $(LDLIBS)

$(OBJECT_DIR)/%.o: $(SRC_DIR)/%.cpp | deps
	@$(PYTHON) -B tools/build_support.py mkdir "$(@D)"
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MMD -MP -c "$<" -o "$@"

copy_runtime: | deps
	@$(PYTHON) -B tools/build_support.py prepare

copy_config: copy_runtime

deps:
	@$(PYTHON) -B package_check.py --fail-on-missing || $(PYTHON) -B package.py

test:
	$(PYTHON) -B -m unittest discover -s tests -v

test-physics: $(PHYSICS_TEST_TARGET)
	$(PYTHON) -B -c "import pathlib, subprocess; executable = pathlib.Path('$(PHYSICS_TEST_TARGET)').resolve(); raise SystemExit(subprocess.call([str(executable)], cwd=executable.parent))"

$(PHYSICS_TEST_TARGET): $(ENGINE_OBJECTS) $(PHYSICS_TEST_OBJECT) | copy_runtime
	$(CXX) $(ENGINE_OBJECTS) $(PHYSICS_TEST_OBJECT) -o "$@" $(LDFLAGS) $(LDLIBS)

$(PHYSICS_TEST_OBJECT): tests/physics_tests.cpp | deps
	@$(PYTHON) -B tools/build_support.py mkdir "$(@D)"
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MMD -MP -c "$<" -o "$@"

clean:
	$(PYTHON) -B tools/build_support.py clean

-include $(OBJECTS:.o=.d)
-include $(PHYSICS_TEST_OBJECT:.o=.d)

.PHONY: all $(MODES) copy_runtime copy_config deps test test-physics clean
