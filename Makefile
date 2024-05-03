CFLAGS=-Wall -O2 -std=c99 -D_POSIX_SOURCE
BINS=apo

.PHONY: all clean

all: $(BINS)

apo: apo.c xml.c rgc.c

apo.c: inc.h

xml.c: inc.h

clean:
	rm -f $(BINS)

