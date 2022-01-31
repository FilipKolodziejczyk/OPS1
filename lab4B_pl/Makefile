CC=gcc
C_FLAGS=-Wall -fsanitize=address,undefined
LIBS=-lrt
FILES=l4solution.o l4solution_util.o
HEADERS=l4solution.h

l4solution: ${FILES} ${HEADERS}
	${CC} ${C_FLAGS} ${LIBS} -o l4solution.out ${FILES}

l4solution.o: l4solution.c ${HEADERS}
	${CC} ${C_FLAGS} ${LIBS} -o l4solution.o -c l4solution.c

l4solution_util.o: l4solution_util.c ${HEADERS}
	${CC} ${C_FLAGS} ${LIBS} -o l4solution_util.o -c l4solution_util.c

.PHONY: clean
clean:
	rm *.out *.o