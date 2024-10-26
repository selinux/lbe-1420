CFLAGS=-Wall
PROG=lbe-142x

HIDLIB=-L. -lhidapi-hidraw -Wl,-rpath,.

all: ${PROG}

${PROG}: lbe-142x.c lbe-142x.h
	gcc ${CFLAGS} -o $@ $?

clean:
	@echo Removing ${PROG}...
	@rm -f ${PROG}


