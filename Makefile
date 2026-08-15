CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c99
TARGET = monopoly
SRCS = main.c board.c players.c finance.c events.c game.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

%.o: %.c types.h board.h players.h finance.h events.h game.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
