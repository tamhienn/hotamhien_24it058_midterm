CC= gcc
CFLAGS= -Wall -Wextra -std=c11 -D_DEFAULT_SOURCE -Iinclude
TARGET= myls

SRCS= src/main.c src/options.c src/listing.c src/sorting.c src/format.c src/utils.c
OBJS= src/main.o src/options.o src/listing.o src/sorting.o src/format.o src/utils.o

all: ${TARGET}

${TARGET}: ${OBJS}
	${CC} ${CFLAGS} -o ${TARGET} ${OBJS}

.c.o:
	${CC} ${CFLAGS} -c ${.IMPSRC} -o ${.TARGET}

clean:
	rm -f ${OBJS} ${TARGET}
