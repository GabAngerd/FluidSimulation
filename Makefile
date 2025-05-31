# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -I./include -g
LDFLAGS = -lglfw -lGL -lm

# Source and object files
SRC = $(wildcard src/*.c) 
OBJ = $(SRC:src/%.c=%.o)

# Output executable
EXEC = main

# Default target
all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) $(LDFLAGS)

%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC)

.PHONY: all clean