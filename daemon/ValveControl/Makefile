TOPDIR  := $(shell cd ..; pwd)
include $(TOPDIR)/Rules.make


OBJS = ValveControl.o AD7705.o TLC5615.o


all: $(OBJS)
	$(CC)  -lm -lpthread -L$(LIBDIR) -lcommon -o $(BINDIR)/ValveControl $(OBJS)

clean:
	rm -f *.o $(BINDIR)/ValveControl *~ core