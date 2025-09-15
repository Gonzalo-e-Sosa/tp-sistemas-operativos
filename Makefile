CC=gcc
CFLAGS=-Wall -Wextra -O2
TARGET=main
BUILD_DIR=build
UTILS_OBJS=$(BUILD_DIR)/csv.o $(BUILD_DIR)/date.o $(BUILD_DIR)/generate.o

all: $(TARGET)

$(TARGET): main.c $(UTILS_OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) main.c $(UTILS_OBJS)

$(BUILD_DIR)/csv.o: utils/csv.c utils/utils.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c utils/csv.c -o $(BUILD_DIR)/csv.o

$(BUILD_DIR)/date.o: utils/date.c utils/utils.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c utils/date.c -o $(BUILD_DIR)/date.o

$(BUILD_DIR)/generate.o: utils/generate.c utils/utils.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c utils/generate.c -o $(BUILD_DIR)/generate.o

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -f $(TARGET)
	rm -rf $(BUILD_DIR)
