CXX = g++
CXXFLAGS = -std=c++17 -Wall -I./src

SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

SOURCES = $(SRC_DIR)/main.cpp
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)