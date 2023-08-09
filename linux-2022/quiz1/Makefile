DONE_PROB = problem1 problem2 problem3 problem4

all: $(DONE_PROB)
problem1: problem1.c
	$(CC) -Wall -o $@ $^
	sudo ./$@

problem2: problem2.c
	$(CC) -Wall -o $@ $^
	sudo ./$@

problem3: problem3.c
	$(CC) -Wall -I../include -o $@ $^
	sudo ./$@

problem4: problem4.c
	$(CC) -Wall -o $@ $^
	sudo ./$@

clean:
	-rm $(DONE_PROB)