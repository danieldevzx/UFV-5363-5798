# Variáveis
CC = gcc
SRC_DIR = src
BIN_DIR = bin

# Lista de todos os arquivos fonte (.c) na pasta src
SOURCES = $(wildcard $(SRC_DIR)/*.c)

# Nome do executável
EXECUTABLE = main

# Comando padrão para compilar
all: $(BIN_DIR)/$(EXECUTABLE)

# Regra de compilação do executável
$(BIN_DIR)/$(EXECUTABLE): $(SOURCES) | $(BIN_DIR)
	$(CC) $(SOURCES) -o $@

# Cria a pasta bin se não existir
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Limpeza
clean:
	rm -rf $(BIN_DIR)
