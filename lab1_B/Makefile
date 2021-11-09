all: l1solution

l1solution: l1solution.c
	gcc -Wall -fsanitize=address,undefined -o l1solution l1solution.c

.PHONY: all clean

clean:
	rm l1solution