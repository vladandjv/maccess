CC= gcc 
INCDIR = -I. -I../toolset_gc/include/
#CFLAGS=  $(INCDIR) -Wall -Werror -pedantic -ggdb3 -fcommon -fPIC -DPIC -z execstack -D_REENTRANT -fstack-protector 
CFLAGS=  $(INCDIR) -Wall -Werror -pedantic -ggdb3 -fPIC -DPIC -z execstack -D_REENTRANT -fstack-protector 
TCAP = -DM_TERMINFO
#SYSLIBS = ../toolset_gc/lib/libgc.a -lncurses -lm -luuid -lpthread
SYSLIBS = -lncurses -lm -luuid -lpthread
RANLIB= ranlib
AR = ar
ARFLAGS = cr

OBJS1 = maccess.o maflush.o mamake.o madelete.o maadd.o \
	mamemory.o mashmlock.o log.o
LIB1 =  maccess.a 

OBJS2 = scrline.o scrcheck.o scrframe.o scrkey.o scrshowsymbols.o screen.o 
LIB2 =  scrlib.a 

OBJS3 =  time.o date.o string.o 
LIB3 = vlib.a

all: $(LIB1) $(LIB2) $(LIB3) example e_load e_print e_rebuild example1 \
	e1_load e1_print e1_rebuild calculate key example_shmrm \
	example1_shmrm btree_parameters 

# targets
$(LIB1): $(OBJS1)
	 $(AR) $(ARFLAGS) $@ $(OBJS1)
	 $(RANLIB) $@

$(LIB2): $(OBJS2)
	 $(AR) $(ARFLAGS) $@ $(OBJS2)
	 $(RANLIB) $@

$(LIB3): $(OBJS3)
	 $(AR) $(ARFLAGS) $@ $(OBJS3)
	 $(RANLIB) $@

calculate: calculate.o 
	   $(CC) $(CFLAGS) $(TCAP) calculate.o $(LIB1) $(LIB2) $(LIB3) \
		   $(SYSLIBS) -o calculate

calculate.o: calculate.c
	   $(CC) $(CFLAGS) -c calculate.c

btree_parameters: btree_parameters.o 
	   $(CC) $(CFLAGS) $(TCAP) btree_parameters.o  \
		   $(SYSLIBS) -o btree_parameters

btree_parameters.o: btree_parameters.c 
	   $(CC) $(CFLAGS) -c btree_parameters.c

key: key.o 
	   $(CC) $(CFLAGS) $(TCAP) key.o $(LIB1) $(LIB2) $(LIB3) $(SYSLIBS) \
           -o key

key.o: key.c
	   $(CC) $(CFLAGS) -c key.c

e_load:    e_load.o 
	   $(CC) $(CFLAGS) $(TCAP) e_load.o log.o $(LIB1) $(SYSLIBS) -o e_load

e_load.o:  e_load.c
	   $(CC) $(CFLAGS) -c e_load.c

e_print:    e_print.o 
	   $(CC) $(CFLAGS) $(TCAP) e_print.o log.o $(LIB1) $(SYSLIBS) -o e_print

e_print.o:  e_print.c
	   $(CC) $(CFLAGS) -c e_print.c

e_rebuild: e_rebuild.o 
	   $(CC) $(CFLAGS) $(TCAP) e_rebuild.o log.o $(LIB1) $(SYSLIBS) \
		   -o e_rebuild

e_reubild.o:  e_rebuild.c
	   $(CC) $(CFLAGS) -c e_rebuild.c

e1_load:   e1_load.o 
	   $(CC) $(CFLAGS) $(TCAP) e1_load.o log.o $(LIB1) $(SYSLIBS) -o e1_load

e1_load.o: e1_load.c
	   $(CC) $(CFLAGS) -c e1_load.c

e1_print:  e1_print.o 
	   $(CC) $(CFLAGS) $(TCAP) e1_print.o log.o $(LIB1) $(SYSLIBS) -o e1_print

e1_rebuild.o: e1_rebuild.c
	   $(CC) $(CFLAGS) -c e1_rebuild.c

e1_rebuild:  e1_rebuild.o 
	   $(CC) $(CFLAGS) $(TCAP) e1_rebuild.o log.o $(LIB1) $(SYSLIBS) \
		   -o e1_rebuild

e1_print.o: e1_print.c
	   $(CC) $(CFLAGS) -c e1_print.c


example:   example.o check.o 
	   $(CC) $(CFLAGS) $(TCAP) example.o check.o $(LIB1) $(LIB2) $(LIB3) \
           $(SYSLIBS) -o example 

example.o: example.c
	   $(CC) $(CFLAGS) -c example.c

check.o:   check.c
	   $(CC) $(CFLAGS) -c check.c

example1:  example1.o check1.o
	   $(CC) $(CFLAGS) $(TCAP) example1.o check1.o $(LIB1) $(LIB2) $(LIB3) \
           $(SYSLIBS) -o example1 

example1.o: example1.c
	   $(CC) $(CFLAGS) -c example1.c

check1.o:  check1.c
	   $(CC) $(CFLAGS) -c check1.c

example_shmrm: example_shmrm.o 
	   $(CC) $(CFLAGS) $(TCAP) example_shmrm.o $(LIB1) $(SYSLIBS) \
           -o example_shmrm

example_shmrm.o: example_shmrm.c
	   $(CC) $(CFLAGS) -c example_shmrm.c

example1_shmrm: example1_shmrm.o 
	   $(CC) $(CFLAGS) $(TCAP) example1_shmrm.o $(LIB1) $(SYSLIBS) \
           -o example1_shmrm

example1_shmrm.o: example1_shmrm.c
	   $(CC) $(CFLAGS) -c example1_shmrm.c

clean:      
	./example_shmrm; ./example1_shmrm; rm -f core core.* *.o *.dbc *.cdx \
		*.log *.tmp $(LIB1) $(LIB2) $(LIB3) example e_load e_print \
		e_rebuild  example1 e1_load  e1_print e1_rebuild calculate \
		key example_shmrm example1_shmrm btree_parameters
cleandata: 
	rm -rf *.dbc *.cdx; ./example_shmrm; ./example1_shmrm
