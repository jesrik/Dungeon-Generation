dungeon: hw1.02.c
	gcc hw1.02.c -o dungeon

w:	hw1.02.c
	gcc -Wall hw1.02.c -o dungeon

clean:
	rm dungeon *~

