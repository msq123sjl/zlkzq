TOPDIR  := $(shell cd ..; pwd)
include $(TOPDIR)/Rules.make

OBJS = autodial.o

all: $(OBJS)
	$(CC)  -lm -lpthread -L$(LIBDIR) -lcommon -lautodial -o $(BINDIR)/autodial $(OBJS)

clean:
	rm -f *.o $(BINDIR)/autodial *~ core