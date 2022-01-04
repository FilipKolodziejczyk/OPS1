CC=gcc
C_FLAGS=-Wall -fsanitize=address,undefined
LIBS=-lrt
FILES=prog21.o prog21_util.o
HEADERS=prog21.h

prog21: ${FILES} ${HEADERS}
	${CC} ${C_FLAGS} ${LIBS} -o prog21.out ${FILES}

prog21.o: prog21.c ${HEADERS}
	${CC} ${C_FLAGS} ${LIBS} -o prog21.o -c prog21.c

prog21_util.o: prog21_util.c ${HEADERS}
	${CC} ${C_FLAGS} ${LIBS} -o prog21_util.o -c prog21_util.c

.PHONY: clean
clean:
	rm *.out *.o