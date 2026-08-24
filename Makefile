CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = processflow
SRC = Main.c

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET) *.o

test: $(TARGET)
	@echo "== Teste basico: task + run =="
	@printf "task listar /bin/ls -l\nrun listar\nexit\n" | ./$(TARGET)