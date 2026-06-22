CC = gcc
CFLAGS = -Wall -Wextra -std=c99

SRCS = main.c slots.c allocation.c billing.c report.c storage.c
OBJS = $(SRCS:.c=.o)
TARGET = parking_system

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
