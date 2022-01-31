CC=gcc
C_FLAGS=-Wall -fsanitize=thread,undefined
LIBS=-lrt -lpthread
FILES=main.o util.o
HEADERS=task4.h

main: ${FILES} ${HEADERS}
	${CC} ${C_FLAGS} ${LIBS} -o main.out ${FILES}

main.o: main.c ${HEADERS}
	${CC} ${C_FLAGS} ${LIBS} -o main.o -c main.c

util.o: util.c ${HEADERS}
	${CC} ${C_FLAGS} ${LIBS} -o util.o -c util.c

.PHONY: clean
clean:
	rm *.out *.o