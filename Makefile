# Panzer War — simple build helpers
# Usage:
#   make          # configure + build into bin/
#   make run      # build and start the game
#   make clean    # remove build files + binary

BUILD_DIR := build
BIN_DIR   := bin
TARGET    := panzer_war

.PHONY: all configure build run clean rebuild

all: build

configure:
	@cmake -S . -B $(BUILD_DIR)

build: configure
	@cmake --build $(BUILD_DIR)
	@mkdir -p $(BIN_DIR)

run: build
	@cd $(BIN_DIR) && ./$(TARGET)

rebuild: clean all

clean:
	@rm -rf $(BUILD_DIR)
	@rm -f $(BIN_DIR)/$(TARGET)
	@echo "cleaned"
