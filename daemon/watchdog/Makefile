TOPDIR  := $(shell cd ..; pwd)
include $(TOPDIR)/Rules.make

OBJS = watchdog.o

all: $(OBJS)
	$(CC) -o $(BINDIR)/watchdog $(OBJS) -L$(LIBDIR) -lcommon -lpthread

clean:
	rm -f *.o $(BINDIR)/watchdog *~ core

