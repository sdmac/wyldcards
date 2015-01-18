COMMON = ../../../common/trunk

INCLUDE = -I../include \
	  -I$(COMMON)/include

CC = g++

CFLAGS = -Wall -O2 -g

COMPILE = $(CC) $(CFLAGS) $(INCLUDE) -c

OBJFILES := $(patsubst %.cc,%.o,$(wildcard *.cc))


all: pusoydos

pusoydos: $(OBJFILES)
	$(CC) $(INCLUDE) -o pusoydos $(OBJFILES) -L$(COMMON)/src -lcommon

%.o: %.cc
	$(COMPILE) -o $@ $<

clean:
	rm *.o pusoydos

.PHONY : clean
