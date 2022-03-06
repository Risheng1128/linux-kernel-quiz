# ------------------------------------------------
# Generic Makefile
# Author: Ri-Sheng Chen
# ------------------------------------------------

CC = gcc
CFLAGS = -O1 -g -Wall -Werror -IInclude

TARGET := quiz1/problem4.c
EXE = $(patsubst %.c,%.out,$(notdir $(TARGET)))

$(EXE): $(TARGET)
	$(CC) $(CFLAGS) -o $@ $^ -lm
	./$(EXE)

clean:
	rm *.out