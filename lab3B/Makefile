CC=gcc
C_FLAGS=-Wall -fsanitize=undefined,thread
LIBS=-lpthread
FILES=l3solution.o l3solution_util.o

l3solution: ${FILES} l3solution.h
	${CC} ${C_FLAGS} ${LIBS} -o l3solution.out ${FILES}

l3solution.o: l3solution.c l3solution.h
	${CC} ${C_FLAGS} ${LIBS} -o l3solution.o -c l3solution.c

l3solution_util.o: l3solution_util.c l3solution.h
	${CC} ${C_FLAGS} ${LIBS} -o l3solution_util.o -c l3solution_util.c

.PHONY: clean
clean:
	rm -f *.out *.o