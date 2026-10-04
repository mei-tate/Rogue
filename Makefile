CC = gcc
CFLAGS = -Wall -Wextra -g -I./include
LIBS = -lncurses
TARGET = rogue
SRCDIR = ./src/
SOURCES = $(wildcard $(SRCDIR)*.c $(SRCDIR)windows/*.c)

all: $(TARGET)

$(TARGET): $(SOURCES) ./include/rogue.h
	$(CC) $(SOURCES) $(CFLAGS) $(LIBS) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
