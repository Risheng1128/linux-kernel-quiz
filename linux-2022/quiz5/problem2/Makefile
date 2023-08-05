CC = gcc
CFLAGS = -O1 -g -Wall -std=gnu11 -Werror -I. -lm
LDFLAGS = -lpthread

TARGET := problem2.c
C_SOURCES := hp.c domain.c

EXE = $(patsubst %.c,%.out,$(TARGET))
OBJECTS = $(patsubst %.c,%.o,$(TARGET))
OBJECTS += $(patsubst %.c,%.o,$(C_SOURCES))

%.o:%.c
	$(CC) $(CFLAGS) -c $^ -o $@

$(EXE): $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS) 

check:
	./$(EXE)

clean:
	-rm *.out *.o