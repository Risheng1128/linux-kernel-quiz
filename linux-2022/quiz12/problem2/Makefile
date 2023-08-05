CC = gcc
CFLAGS = -Wall -std=c11 -D_GNU_SOURCE -I.

TARGET = test

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $@ $^ -lpthread
	./$@

clean:
	rm $(TARGET)