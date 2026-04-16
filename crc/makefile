CC := gcc
CFLAGS := -Wall -Wextra -g $(shell pkg-config --cflags libserialport)
LDFLAGS := $(shell pkg-config --libs libserialport)

TARGET := test_crc
SRCS := test_crc.c voltronic_crc.c
OBJS := $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

%.o: %.c bla.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean