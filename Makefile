CC=gcc
CFLAGS=-Wall -Wextra -O2

SRC_DIR=src
INCLUDE_DIR=include
BUILD_DIR=build
LIB_DIR=lib
TARGET=$(BUILD_DIR)/main

SRCS=$(wildcard $(SRC_DIR)/*.c)
OBJS=$(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
LIBS=$(LIB_DIR)/libstock.a


.PHONY: all clean


all: $(TARGET)

$(TARGET): $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -o $@ $(OBJS) $(LIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)


$(LIB_DIR)/libstock.a:
	$(MAKE) -C $(LIB_DIR)

lib:
	$(MAKE) -C $(LIB_DIR)

clean:
	rm -rf $(BUILD_DIR)
	$(MAKE) -C $(LIB_DIR) clean

