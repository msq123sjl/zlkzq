TOPDIR  := $(shell cd ..; pwd)
include $(TOPDIR)/Rules.make

OBJS = config_para.o

all: $(OBJS)
	$(CC)  -lm -lpthread -L$(LIBDIR) -lcommon -o $(BINDIR)/config_para $(OBJS)

clean:
	rm -f *.o $(BINDIR)/config_para *~ core