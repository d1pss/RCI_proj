
# Nome do executável
TARGET = OWR

# Compilador e ficheiros fonte
CC = gcc
SRCS = main.c cmd_aux.c cmd.c routing.c server_client_comunication.c

# Flags base (comuns a ambos os modos)
# Mantemos o POSIX aqui para o getaddrinfo funcionar sempre
BASE_CFLAGS = -Wall -I.

# Regra por defeito (Otimizada com -O3)
all: CFLAGS = $(BASE_CFLAGS) -O3
all: $(TARGET)

# Regra de Debug (com -g para usar o gdb)
debug: CFLAGS = $(BASE_CFLAGS) -g
debug: $(TARGET)

$(TARGET):
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

# Limpeza
clean:
	rm -f $(TARGET)