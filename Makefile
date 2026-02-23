BUILD_DIR := build

TARGET := $(BUILD_DIR)/libutf8.a

SRC := utf-8.c
OBJ := $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRC))

CC := gcc
CFLAGS := -std=gnu11 -Wall -Wextra -O2

all: $(TARGET)

debug: CFLAGS += -g
debug: all

.PHONY: debug all clean

$(BUILD_DIR):
	@mkdir -p $@

$(BUILD_DIR)/%.o: $(SRC) | $(BUILD_DIR)
	@$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJ)
	@ar rcs $@ $^
	@echo "-> $@"

clean:
	rm -rf $(BUILD_DIR)