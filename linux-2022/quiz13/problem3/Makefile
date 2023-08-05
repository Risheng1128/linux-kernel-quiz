CC = gcc

HELLO = hello

hello: hello.c
	$(CC) -Os $^ -o $@
	strip -s $@

execelf: execelf.c
	$(CC) -Wall -o $@ $^
	./$@ hello

loader: loader.c
	$(CC) -Wall $^ -o $@ 
	cat hello >> $@
	./$@

clean:
	rm hello execelf loader