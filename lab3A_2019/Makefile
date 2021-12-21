CC=gcc
C_FLAGS=-Wall -fsanitize=undefined,thread
LIBS=-lpthread -lm
FILES=threadtree.o threadtree_util.o

threadtree: ${FILES} threadtree.h
	${CC} ${C_FLAGS} ${LIBS} -o threadtree.out ${FILES}

threadtree.o: threadtree.c threadtree.h
	${CC} ${C_FLAGS} ${LIBS} -o threadtree.o -c threadtree.c

threadtree_util.o: threadtree_util.c threadtree.h
	${CC} ${C_FLAGS} ${LIBS} -o threadtree_util.o -c threadtree_util.c

.PHONY: clean
clean:
	rm -f *.out *.o