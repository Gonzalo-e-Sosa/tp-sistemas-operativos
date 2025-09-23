
CC=gcc
CFLAGS=-Wall -Wextra -O2

INCLUDE_DIR=../include
OBJ_DIR=obj
SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,$(OBJ_DIR)/%.o,$(SRCS))
TARGET=libstock.a


all: $(OBJ_DIR) $(TARGET)


$(TARGET): $(OBJS)
	ar rcs $@ $^


$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)


clean:
	rm -rf $(OBJ_DIR) $(TARGET)
