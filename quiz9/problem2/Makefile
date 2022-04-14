CC = gcc
CFLAG = -Wall -O1 -std=gnu11 -I.
LDFLAGS = -lpthread
TARGET = test

$(TARGET).out: $(TARGET).c
	$(CC) $(CFLAG) $^ -o $@ $(LDFLAGS)
	./$@

clean:
	rm *.out