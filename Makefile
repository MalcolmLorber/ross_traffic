LIBDIR  := ../../ross
CC	:= $(shell $(LIBDIR)/ross-config --cc)
CFLAGS  := -g -Wall -I.
OPTIONS := $(shell $(LIBDIR)/ross-config --cflags)
LDFLAGS := $(shell $(LIBDIR)/ross-config --ldflags)

all: traffic
clean:
	rm -f traffic
	rm -f *.o *~ core*


airport: traffic.c $(LIBDIR)/libROSS.a $(LIBDIR)/ross-config
	$(CC) $(CFLAGS) $(OPTIONS) traffic.c -o traffic $(LDFLAGS)
