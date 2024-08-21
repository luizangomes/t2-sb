CC=gcc
CFLAGS=-std=c99 -Wall -Wextra

DIASSEMBLER_SRC=diassembler.c
DIASSEMBLER_OBJ=$(DIASSEMBLER_SRC:.c=.o)
DIASSEMBLER_TARGET=tradutor

.PHONY: all clean

all: $(DIASSEMBLER_TARGET)

$(DIASSEMBLER_TARGET): $(DIASSEMBLER_OBJ)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(DIASSEMBLER_TARGET) $(DIASSEMBLER_OBJ)