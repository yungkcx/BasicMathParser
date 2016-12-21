CC=gcc
CFLAGS= -Wall -L.
AR=ar
ARFLAGS= rv
LIBBM=libbm.a
LIBS= -lbm
RM=rm -f
TEST=test

${LIBBM}: math_parser.o
	${AR} ${ARFLAGS} ${LIBBM} $^

test: test.o ${LIBBM}
	${CC} -o $@ $^ ${CFLAGS} ${LIBS}

.PHONY: clean
clean:
	${RM} ${LIBBM} ${TEST} *.o
