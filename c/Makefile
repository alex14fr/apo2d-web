CFLAGS=-Wall -O3 -D_POSIX_SOURCE
BINS=apo rgc

.PHONY: all clean

all: $(BINS)

apo: apo.c xml.c rgc.c

rgc:
	ln -s apo rgc

apo.c: inc.h

xml.c: inc.h

clean:
	rm -f $(BINS)

