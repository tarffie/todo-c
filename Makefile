# User-space Makefile
CC = cc
CFLAGS = -Wall -Wextra -std=c11 -g -I/usr/local/include
LDFLAGS = -L/usr/local/lib -lsqlite3

# Project structure
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

# Targets
TARGET = $(BIN_DIR)/todo-c

# Sources and objects
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

# Installation paths
LOCAL_OPT = $(HOME)/.local/opt/todo-c
LOCAL_BIN = $(HOME)/.local/bin

# Default target
all: setup $(TARGET)

# Link target
$(TARGET): $(OBJS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Compile objects
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

# Create directories
setup:
	@mkdir -p $(BUILD_DIR) $(BIN_DIR)

# Install to user space
install: $(TARGET)
	@echo "Installing to user space"
	@mkdir -p $(LOCAL_OPT) $(LOCAL_BIN)
	@cp $(TARGET) $(LOCAL_OPT)/
	@chmod 755 $(LOCAL_OPT)/todo-c
	@ln -sf $(LOCAL_OPT)/todo-c $(LOCAL_BIN)/todo-c
	@echo "Installed. Ensure $(LOCAL_BIN) is in your PATH"
	@echo "Try: export PATH=\$$PATH:$(LOCAL_BIN)"

uninstall:
	@rm -f $(LOCAL_OPT)/todo-c
	@rm -f $(LOCAL_BIN)/todo-c
	@rmdir --ignore-fail-on-non-empty $(LOCAL_OPT)
	@echo "Uninstalled"

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: all setup clean install uninstall
