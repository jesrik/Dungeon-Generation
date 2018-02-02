hw1.02: hw1.02.c
	gcc hw1.02.c -o hw1.02

w:	hw1.02.c
	gcc -Wall hw1.02.c -o hw1.02

clean:
	rm hw1.02 *~

