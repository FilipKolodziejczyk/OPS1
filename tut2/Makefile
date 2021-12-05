CC=gcc
C_FLAGS=-Wall -fsanitize=address,undefined

prog13: prog13.c
	${CC} ${C_FLAGS} -o prog13.out prog13.c

prog14: prog14.c
	${CC} ${C_FLAGS} -o prog14.out prog14.c

.PHONY: clean
clean:
	rm -f *.out