CC=gcc
CFLAGS=-pthread -std=c2x
LDFLAGS=-pthread -std=c2x

SRCDIR=.
LIBDIR=$(SRCDIR)/lib
BINDIR=.

SOURCES=$(SRCDIR)/main.c $(LIBDIR)/queue.c $(LIBDIR)/ping.c $(LIBDIR)/listen.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=$(BINDIR)/udping

.PHONY: all clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(EXECUTABLE) $(OBJECTS)