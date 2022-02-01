CC=gcc
C_FLAGS=-Wall -fsanitize=address,undefined
LIBS=-lrt

main: main.c ${HEADERS}
	${CC} ${C_FLAGS} ${LIBS} -o main.out main.c

.PHONY: clean
clean:
	rm *.out