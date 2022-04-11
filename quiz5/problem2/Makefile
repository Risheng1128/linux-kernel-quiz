CC = gcc
CFLAGS = -O1 -g -Wall -std=c11 -Werror -I.

TARGET := problem2.c
C_SOURCES := hp.c domain.c
EXE = $(patsubst %.c,%.out,$(TARGET))

OBJECTS = $(patsubst %.c,%.o,$(C_SOURCES))
vpath %.c $(C_SOURCES)

%.o:%.c
	$(CC) $(CFLAGS) -c $^ -o $@

$(EXE): $(TARGET) $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ -lm
	./$(EXE)

clean:
	-rm *.out *.o