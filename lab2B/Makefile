CC=gcc
C_FLAGS=-Wall -fsanitize=address,undefined
FILES=l2solution.o l2solution_io.o l2solution_sig.o

l2solution.out: ${FILES} l2solution.h
	${CC} ${C_FLAGS} -o l2solution.out ${FILES}

l2solution.o: l2solution.c l2solution.h
	${CC} ${C_FLAGS} -o l2solution.o -c l2solution.c

l2solution_io.o: l2solution_io.c l2solution.h
	${CC} ${C_FLAGS} -o l2solution_io.o -c l2solution_io.c

l2solution_sig.o: l2solution_sig.c l2solution.h
	${CC} ${C_FLAGS} -o l2solution_sig.o -c l2solution_sig.c

.PHONY: clean
clean:
	rm -f ${FILES} l2solution.out *.txt