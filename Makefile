CC=gcc
CFLAGS=-Wall -Wextra -O2
TARGET=main
BUILD_DIR=build
UTILS_SRCS=$(wildcard utils/*.c)
UTILS_OBJS=$(patsubst utils/%.c,$(BUILD_DIR)/%.o,$(UTILS_SRCS))

all: $(TARGET)

$(TARGET): main.c $(UTILS_OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) main.c $(UTILS_OBJS)

$(BUILD_DIR)/%.o: utils/%.c utils/utils.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -f $(TARGET) $(UTILS_OBJS)
	rm -rf $(BUILD_DIR)
