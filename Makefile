# ------------------------------------------------
# Generic Makefile
# Author: Ri-Sheng Chen
# ------------------------------------------------

CC = gcc
CFLAGS = -O1 -g -Wall -Werror -IInclude

TARGET := quiz2/problem3.c
EXE = $(patsubst %.c,%.out,$(notdir $(TARGET)))

$(EXE): $(TARGET)
	$(CC) $(CFLAGS) -o $@ $^ -lm

run:
	./$(EXE)

disasm: $(EXE)
	objdump -d -M att $^ > $(patsubst %.out,%.d,$(EXE))

clean:
	-rm *.out *.d *.png out