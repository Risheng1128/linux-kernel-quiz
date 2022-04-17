CC = gcc
CFLAG = -Wall -O0 -g -I.
TARGET = time

$(TARGET): $(TARGET).c
	$(CC) $(CFLAG) $^ -o $@

check: $(TARGET)
	./$^

# measure time
mtime: $(TARGET)
	python3 eliminate.py
	gnuplot time.gp

clean:
	-@rm $(TARGET) out final *png