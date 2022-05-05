CC = gcc
CFLAGS = -std=c11 -D_POSIX_C_SOURCE=200809L

TARGET = mmap-filecopy

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $@ $^
	./$@ in out

clean:
	rm $(TARGET)