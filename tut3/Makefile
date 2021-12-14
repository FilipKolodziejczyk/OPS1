CC=gcc
C_FLAGS=-Wall -fsanitize=address,undefined
LIBS=-lpthread -lm

prog17: prog17.c
	${CC} ${C_FLAGS} ${LIBS} -o prog17.out prog17.c

.PHONY: clean
clean:
	rm -f *.out