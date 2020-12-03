CC= gcc 
INCDIR = -I. -I../ -I../../toolset_gc/include/
CFLAGS=  $(INCDIR) -Wall -Werror -pedantic -ggdb3 -fPIC -DPIC -z execstack -D_REENTRANT -fstack-protector 
TCAP = -DM_TERMINFO
SYSLIBS = -lm -luuid 
LIB1 =  maccess.a 

all: e2_origin_load e2_origin_print e2_origin_rebuild \
	e2_ip_address_load e2_ip_address_print e2_ip_address_rebuild \
	e2_auth_id_allowed_load e2_auth_id_allowed_print \
	e2_auth_id_allowed_rebuild e2_resource_load e2_resource_print\
	e2_resource_rebuild example2_shmrm

# targets
e2_origin_load:  e2_origin_load.o 
	   $(CC) $(CFLAGS) $(TCAP) e2_origin_load.o ../$(LIB1) \
		   $(SYSLIBS) -o e2_origin_load 

e2_origin_load.o: e2_origin_load.c
	   $(CC) $(CFLAGS) -c e2_origin_load.c

e2_origin_print:  e2_origin_print.o 
	   $(CC) $(CFLAGS) $(TCAP) e2_origin_print.o ../$(LIB1) \
		   $(SYSLIBS) -o e2_origin_print 

e2_origin_print.o: e2_origin_print.c
	   $(CC) $(CFLAGS) -c e2_origin_print.c

e2_origin_rebuild:  e2_origin_rebuild.o 
	   $(CC) $(CFLAGS) $(TCAP) e2_origin_rebuild.o ../$(LIB1) \
		   $(SYSLIBS) -o e2_origin_rebuild 

e2_origin_rebuild.o: e2_origin_rebuild.c
	   $(CC) $(CFLAGS) -c e2_origin_rebuild.c

e2_ip_address_load:  e2_ip_address_load.o 
	   $(CC) $(CFLAGS) $(TCAP) e2_ip_address_load.o ../$(LIB1) \
		   $(SYSLIBS) -o e2_ip_address_load 

e2_ip_address_load.o: e2_ip_address_load.c
	   $(CC) $(CFLAGS) -c e2_ip_address_load.c

e2_ip_address_print:  e2_ip_address_print.o 
	   $(CC) $(CFLAGS) $(TCAP) e2_ip_address_print.o ../$(LIB1) \
		   $(SYSLIBS) -o e2_ip_address_print 

e2_ip_address_print.o: e2_ip_address_print.c
	   $(CC) $(CFLAGS) -c e2_ip_address_print.c

e2_ip_address_rebuild:  e2_ip_address_rebuild.o 
	   $(CC) $(CFLAGS) $(TCAP) e2_ip_address_rebuild.o ../$(LIB1) \
		   $(SYSLIBS) -o e2_ip_address_rebuild 

e2_ip_address_rebuild.o: e2_ip_address_rebuild.c
	   $(CC) $(CFLAGS) -c e2_ip_address_rebuild.c

e2_auth_id_allowed_load:  e2_auth_id_allowed_load.o 
	   $(CC) $(CFLAGS) $(TCAP) e2_auth_id_allowed_load.o ../$(LIB1) \
		   $(SYSLIBS) -o e2_auth_id_allowed_load 

e2_auth_id_allowed_load.o: e2_auth_id_allowed_load.c
	   $(CC) $(CFLAGS) -c e2_auth_id_allowed_load.c

e2_auth_id_allowed_print:  e2_auth_id_allowed_print.o 
	   $(CC) $(CFLAGS) $(TCAP) e2_auth_id_allowed_print.o ../$(LIB1) \
		   $(SYSLIBS) -o e2_auth_id_allowed_print 

e2_auth_id_allowed_print.o: e2_auth_id_allowed_print.c
	   $(CC) $(CFLAGS) -c e2_auth_id_allowed_print.c

e2_auth_id_allowed_rebuild:  e2_auth_id_allowed_rebuild.o 
	   $(CC) $(CFLAGS) $(TCAP) e2_auth_id_allowed_rebuild.o ../$(LIB1) \
		   $(SYSLIBS) -o e2_auth_id_allowed_rebuild 

e2_auth_id_allowed_rebuild.o: e2_auth_id_allowed_rebuild.c
	   $(CC) $(CFLAGS) -c e2_auth_id_allowed_rebuild.c

e2_resource_load:  e2_resource_load.o 
	   $(CC) $(CFLAGS) $(TCAP) e2_resource_load.o ../$(LIB1) \
		   $(SYSLIBS) -o e2_resource_load 

e2_resource_load.o: e2_resource_load.c
	   $(CC) $(CFLAGS) -c e2_resource_load.c

e2_resource_print:  e2_resource_print.o 
	   $(CC) $(CFLAGS) $(TCAP) e2_resource_print.o ../$(LIB1) \
		   $(SYSLIBS) -o e2_resource_print 

e2_resource_print.o: e2_resource_print.c
	   $(CC) $(CFLAGS) -c e2_resource_print.c

e2_resource_rebuild:  e2_resource_rebuild.o 
	   $(CC) $(CFLAGS) $(TCAP) e2_resource_rebuild.o ../$(LIB1) \
		   $(SYSLIBS) -o e2_resource_rebuild 

e2_resource_rebuild.o: e2_resource_rebuild.c
	   $(CC) $(CFLAGS) -c e2_resource_rebuild.c

example2_shmrm: example2_shmrm.o 
	   $(CC) $(CFLAGS) $(TCAP) example2_shmrm.o ../$(LIB1) $(SYSLIBS) \
           -o example2_shmrm

example2_shmrm.o: example2_shmrm.c
	   $(CC) $(CFLAGS) -c example2_shmrm.c

clean:      
	./example2_shmrm; rm -f core core.* *.o *.dbc *.cdx *.log *.tmp \
		e2_origin_load e2_origin_print e2_origin_rebuild \
		e2_ip_address_load e2_ip_address_print e2_ip_address_rebuild \
		e2_auth_id_allowed_load  e2_auth_id_allowed_print \
	        e2_auth_id_allowed_rebuild e2_resource_load e2_resource_print \
		e2_resource_rebuild calculate example2_shmrm 
cleandata: 
	rm -rf *.dbc *.cdx; ./example2_shmrm 
