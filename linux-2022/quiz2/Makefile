DONE_PROB = problem1 problem2 problem3 problem4 problem5 problem6 problem7

all: $(DONE_PROB)
problem1: problem1.c
	$(CC) -Wall -o $@ $^
	sudo ./$@

problem2: problem2.c
	$(CC) -Wall -o $@ $^
	sudo ./$@

problem3: problem3.c
	$(CC) -Wall -o $@ $^
	sudo ./$@ > out
	gnuplot p3_time.gp

problem4: problem4.c
	$(CC) -Wall -o $@ $^
	sudo ./$@

problem5: problem5.c
	$(CC) -Wall -I../include -o $@ $^
	sudo ./$@

problem6: problem6.c
	$(CC) -Wall -o $@ $^
	sudo ./$@

problem7: problem7.c
	$(CC) -Wall -o $@ $^
	sudo ./$@

clean:
	-rm $(DONE_PROB) out time.png