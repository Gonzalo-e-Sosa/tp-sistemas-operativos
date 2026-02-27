CC = gcc
CFLAGS = -Iinclude -O2 -MMD -MP

# Directorios
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

# Ejecutables
EXECUTABLES = main server client
TARGETS = $(addprefix $(BIN_DIR)/, $(EXECUTABLES))

# --- Fuentes ---
ALL_SRCS = $(wildcard $(SRC_DIR)/*.c)

# main usa todos los .c excepto server y client
MAIN_SRCS = $(filter-out $(SRC_DIR)/server.c $(SRC_DIR)/client.c, $(ALL_SRCS))
MAIN_OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(MAIN_SRCS))

# server y client individuales
SERVER_SRC = $(SRC_DIR)/server.c
CLIENT_SRC = $(SRC_DIR)/client.c

# Dependencias
DEPS = $(MAIN_OBJS:.o=.d)

# --- Regla por defecto ---
all: $(TARGETS)

# Alias para poder usar `make main`, `make server`, etc.
$(EXECUTABLES): %: $(BIN_DIR)/%
	@true

# Compilar main (usa pthread)
$(BIN_DIR)/main: $(MAIN_OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -lpthread -o $@ $^

# Compilar server (solo server.c con -lpthread)
$(BIN_DIR)/server: $(SERVER_SRC) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $< -lpthread

# Compilar client (solo client.c sin pthread)
$(BIN_DIR)/client: $(CLIENT_SRC) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $<

# Regla general para compilar .c a .o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Crear carpetas si no existen
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Limpieza
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Incluir dependencias auto-generadas
-include $(DEPS)
