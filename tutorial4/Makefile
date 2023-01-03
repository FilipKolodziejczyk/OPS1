CC=gcc
C_FLAGS=-Wall -fsanitize=address,undefined
LIBS=-lrt

tutorial4: tutorial4.c
	${CC} ${C_FLAGS} ${LIBS} -o tutorial4.out tutorial4.c

.PHONY: clean
clean:
	rm tutorial4.out