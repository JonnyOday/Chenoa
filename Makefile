CC = gcc
SRC = main.c
LIBS = -lcurl -lm
CFLAGS = -g -pedantic
TARGET = chenoa

all: $(SRC)
	$(CC) $(SRC) $(LIBS) $(CFLAGS) -o $(TARGET)

clean: 
	rm $(TARGET)


.PHONY: all clean
