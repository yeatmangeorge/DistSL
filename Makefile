CC := clang
C_STANDARD := c11
C_WARNINGS := \
	-Wall \
	-Wextra \
	-Wpedantic \
	-Werror \
	-Wshadow \
	-Wconversion \
	-Wformat=2 \
	-Wundef \
	-Wnull-dereference \
	-Wimplicit-fallthrough
C_FLAGS := -std=$(C_STANDARD) $(C_WARNINGS) 
BUILD_DIR := .build
SRC_DIR := src
C_SRCS := $(shell find $(SRC_DIR) -name '*.c')
ENTRY_POINT := $(SRC_DIR)/main.c
BUILD_TARGET := $(BUILD_DIR)/dsp

.PHONY: all build

all: gen-folder build

build:
	$(CC) $(C_FLAGS) -o $(BUILD_TARGET) $(ENTRY_POINT)
gen-folder:
	mkdir -p $(BUILD_DIR)

