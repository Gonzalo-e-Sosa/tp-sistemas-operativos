CC=gcc
CFLAGS=-I./include -Wall -Wextra -O2
SRC= main.c src/control_de_parametros.c src/utils.c
TARGET=main

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)
