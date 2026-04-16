# Makefile
# ========
# MP Serial Command Sender: src/mp_serial_cmd.c -> build/mp_serial_cmd.o -> ./mp_serial_cmd
#
# Targets:
#   make / all      : Build release
#   make debug      : Build debug (-g)
#   make clean      : Remove build/ and mp_serial_cmd
#   make install    : Install to /usr/local/bin (sudo)
#   make info       : Show SOURCES/OBJECTS/CFLAGS
#   make format     : clang-format sources/headers
#
# Author: Assistant
# Date: January 2026
# Changes:
# - Renamed to mp_serial_cmd (target/source/object)
# - Explicit SOURCES/OBJECTS, order-only dirs
# - .PRECIOUS objects (preserve in build/)
# - Explicit compile rule + pattern

TARGET      := test_mp_serial
SRC_DIR     := src
INC_DIR     := includes
LIB_DIR     := lib
OBJ_DIR   := obj
INSTALL_DIR := /usr/local/bin

SOURCES     := $(SRC_DIR)/mp_serial_cmd.c
OBJECTS     := $(OBJ_DIR)/mp_serial_cmd.o

CC          := gcc
CFLAGS      := -Wall -Wextra -std=c99 -O2 -I$(INC_DIR)
LDFLAGS     := -L$(LIB_DIR)
DEBUGFLAGS  := -g -O0 -DDEBUG -Wall -Wextra -std=c99 -O2 -I$(INC_DIR)

.PRECIOUS: $(OBJECTS) $(OBJ_DIR)/%.o

# all objects are dependent on 
$(OBJECTS): | $(OBJ_DIR)

$(OBJ_DIR) $(LIB_DIR):
	mkdir -p $@

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

$(OBJ_DIR)/mp_serial_cmd.o: $(SRC_DIR)/mp_serial_cmd.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/mp_serial_cmd.c -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

debug: CFLAGS += $(DEBUGFLAGS)
debug: $(TARGET)

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

install: $(TARGET)
	install -m 755 $(TARGET) $(INSTALL_DIR)

uninstall:
	rm -f $(INSTALL_DIR)/$(TARGET)

format:
	find $(SRC_DIR) $(INC_DIR) -name "*.c" -o -name "*.h" | xargs clang-format -i

info:
	@echo "TARGET:  $(TARGET)"
	@echo "SOURCES: $(SOURCES)"
	@echo "OBJECTS: $(OBJECTS)"
	@echo "CFLAGS:  $(CFLAGS)"
	@echo "LDFLAGS: $(LDFLAGS)"

all: $(TARGET)

.PHONY: all debug clean install uninstall format info
