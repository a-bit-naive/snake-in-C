CC = gcc
CFLAGS = -Wall -Wextra
LDLIBS = -lraylib

TARGET = bin/main
SRC = ./src/main.c ./src/stack.c
OBJ = $(SRC:.c=.o)

$(TARGET): $(OBJ)
	mkdir -p bin
	$(CC) $(OBJ) -o $(TARGET) $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: run clean
