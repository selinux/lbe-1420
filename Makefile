PROG=lbe-142x

CFLAGS=-Wall

all: ${PROG}

${PROG}: lbe-142x.c lbe-142x.h
	gcc ${CFLAGS} -o $@ $?

clean:
	@echo Removing ${PROG}...
	@rm -f ${PROG}


