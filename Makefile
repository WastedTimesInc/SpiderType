# Compiler
CC = gcc

# Compiler and linker flags
CFLAGS = -I/usr/include -Wall -O2 -ggdb
LDFLAGS = -L/usr/lib -lraylib -lm -lpthread -ldl -lrt -lGL -lX11

# Target name
TARGET = SpiderType

# Source files
SRC = src/main.c src/gap_buffer.c

# Build rule
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

# Clean rule
clean:
	rm -f $(TARGET)
