CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -O2 -MMD -MP

# Directorios
SRC_DIR = src
BUILD_DIR = build
TARGET = main

# Archivos fuente y objetos
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))
MAIN = main.c

# Archivos de dependencias generados por -MMD
DEPS = $(OBJS:.o=.d)

# Regla por defecto
all: $(TARGET)

# Enlazado final
$(TARGET): $(MAIN) $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compilación de .c a .o con generación de dependencias
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Crear build si no existe
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Limpieza
clean:
	rm -f $(TARGET)
	rm -rf $(BUILD_DIR)

# Incluir dependencias auto-generadas
-include $(DEPS)
