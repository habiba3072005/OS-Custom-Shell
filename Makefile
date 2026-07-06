# CC: The compiler to be used
CC = gcc


CFLAGS = -Wall -g

# TARGET: The name of the final executable file
TARGET = myShell

# SRC: The source code file
SRC = myShell.c

# runs when you just type 'make'
all: $(TARGET)

# Link the object files and create the executable
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

# 'run' target: Compiles the code and then executes the shell immediately
run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)
