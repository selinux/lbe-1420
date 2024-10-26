CFLAGS=-g -W
PROG=lbe-142x

HIDLIB=-L. -lhidapi-hidraw -Wl,-rpath,.

all: ${PROG}

libhidapi-hidraw.so:
	ln -s libhidapi-hidraw.so.0 libhidapi-hidraw.so

${PROG}: lbe-142x.c lbe-142x.h
	gcc ${CFLAGS} -o $@ $?

clean:
	@echo Removing ${PROG}...
	@rm -f ${PROG}


