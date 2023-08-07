problem1: problem1.c
	$(CC) -Wall -o $@ $^
	sudo ./$@

problem2: problem2.c
	$(CC) -Wall -o $@ $^
	sudo ./$@

problem3: problem3.c
	$(CC) -Wall -o $@ $^ -lpthread
	sudo ./$@ -t -v

clean:
	-rm problem1 problem2 problem3