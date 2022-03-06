# ------------------------------------------------
# Generic Makefile
# Author: Ri-Sheng Chen
# ------------------------------------------------

CC = gcc
CFLAGS = -O1 -g -Wall -Werror -IInclude
TARGET := quiz1/problem3.c
EXE = $(patsubst %.c,%.out,$(notdir $(TARGET)))

all: $(EXE)
$(EXE): $(TARGET)
	$(CC) $(CFLAGS) -o $@ $^ -lm
	./$(EXE)

clean:
	rm $(EXE)