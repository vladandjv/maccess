![Cover](https://github.com/vladandjv/maccess/blob/master/.resources/db-engine.png)

MACCESS - B+ tree database file system with shared memory caching 
--------------------------------------------------------------------------


Author: Vladan Djordjevic
	vladandjv@gmail.com                 
Copyright 2020 Vladan Djordjevic
--------------------------------------------------------------------------
DESCRIPTION

MACCESS is a B+ binary tree implementation in C. It is a relational 
database file system that can work in a multiuser environment. It uses  
shared memory segment for inter process communication and  has a high 
percentage of hits in RAM memory (usually over 60% on 100  million 
records), which makes it very fast.

The Locking system locks the shared memory. It is based on semaphores and 
doesn't touch a hard disk. It is extremely fast and don't leave any 
possibility for dead lock, so there is no need for Edsger Dijkstra's 
banker's algorithm.

Maccess is tested on 100 million records for the data file with its index 
file, but there is no reason why it couldn't work with billions of  
records. The sizes of files are not limited. 

--------------------------------------------------------------------------
EXAMPLE PROGRAMS

The program for displaying current parameters, calculating parameters 
which are interdependent and calculating shared memory segment size  
is called btree_parameters. Also there is the precompiled character  
based UI interactive program called calculate-x86_64.AppImage 
(chmod +x to make it executable) which does the calculation of database 
efficiency and resources requirements base on entered parameters.

The parameters obtained by using the btree_parameters program are located 
in file maccess.h and there they can be changed.

Testing programs are example, e_load, e_print, e_delete ... They can run  
simultaneously. 

It is possible to compile a single-user version and without shared memory. 
Only uncomment: #define SINGLE_USER_NO_SHARED_MEMORY in defs.h.

Record with key used in testing programs is described in /docs as 
Record_example.txt.

example-x86_64.AppImage (chmod +x to make it executable) is the precompiled 
character based UI.  It is a very simple sample for using maccess. It has 
only one key.

e_load.c, e_print.c and e_delete.c - Command line programs for testing  
massive number of records, and also can be used for testing memory leak  
with valgrind or similar tool. See the headers of the source files for
more detailes.

e_create.c -Creates the database files. Must be run first!
e_test.c - Shows B+ tree parameters and adds only 10 records.

Deleting shared memory, made specifically for the testing programs,  
is with e_shmrm. It deletes shared memory segments and semaphores 
for the testing programs. Their special codes are defined in defs.h. 

e_rebuild_sh - The shell script for rebuilding the database.  

e_clean_db.sh - The shell script for deleting the database files and shared 
                memory segments. Call it with "source ./e_clean_db.sh".

There is the log file called: example.log.

Tested with GCC and Clang compilers.

A detailed description of the constants and the ways of their calculation 
can be read in ./docs/maccess_cons_types_vars.txt. Detailed description  
of all functions of the maccess package is in ./docs/maccess_functions.txt. 

The theory of the binary tree and the version B+ (the inventors are  
Rudolf Bayer and Edward Meyers McCreight) which is applied in this  
package, can be read in the B+tree_theory.txt file. 

--------------------------------------------------------------------------
