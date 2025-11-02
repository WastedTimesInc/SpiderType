# Compiler
CC = gcc

# Compiler and linker flags
CFLAGS = -I/usr/include -Wall -O2 -ggdb
LDFLAGS = -L/usr/lib -lraylib -lm -lpthread -ldl -lrt -lGL -lX11

# Target name
TARGET = SpiderType
TESTTARGET = test

# Source files
SRC = src/main.c 

TESTSRC = src/test.c
# Build rule
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

# Clean rule
clean:
	rm -f $(TARGET)

test : $(TESTSRC)
	$(CC) $(CFLAGS) -o $(TESTTARGET) $(TESTSRC) $(LDFLAGS)
