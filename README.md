![Cover](https://github.com/vladandjv/maccess/blob/master/.resources/db-engine.png)

MACCESS - B+ tree database file system with shared memory caching 
--------------------------------------------------------------------------


Author: Vladan Djordjevic
	vladandjv@gmail.com                 

--------------------------------------------------------------------------
DESCRIPTION

MACCESS is a B+ binary tree implementation in C. It is a relation 
database file system that can work in a multiuser environment. It uses 
memory cashing based on a least-recently-used (LRU) strategy. Cache is 
stored in a shared memory segment for inter process communication and 
has a high percentage of hits in RAM memory (usually over 60% on 100 
million records), which makes it very fast.

Locking system is a simple one, based on shared memory too and it doesn't 
touch a hard disk. It is extremely fast and don't leave any possibility 
for dead lock, so there is no need for Edsger Dijkstra's banker's 
algorithm.

It is tested on 100 million record for each data file with its index
files, but there is no reason why it couldn't work with billions of
records.

--------------------------------------------------------------------------
EXAMPLE PROGRAMS

The program for adjusting the parameters and database efficiency is 
called calculate.

The parameters obtained by using the calculate program are located in 
file maccess.h and there they can be changed.

The program for displaying current parameters, calculating parameters 
which are interdependent and calculating shared memory segment size 
is called btree_parameters.

Testing programs are example, example1, example2 ... They can run 
simultaneously. example2 is in the separate folder. 

Records with keys used in testing programs are described in /docs as 
Record_example.txt, Record_example1.txt and Records_example2.txt 
(it has 4 records).

EXAMPLE is a very simple sample and template for using maccess. It
has only one key.
example - Character based UI (garbage collector is a must, otherwise
there will be memory leak).
e_load.c, e_print.c and e_rebuild.c - command line programs for
testing massive number of records. Don't require garbage collector and 
also can be used for testing memory leak with valgrind or similar tool.

EXAMPLE1 is a more complex sample and template for using maccess. It
has four keys.
example1.c - Character based UI (garbage collector is must, otherwise
there will be memory leak).
e1_load.c, e1_print.c and e1_rebuild.c - command line programs for
testing massive number of records. Don't require garbage collector and 
also can be used for testing memory leak with valgrind or similar tool.

EXAMPLE2 is even more complex sample and template for using maccess.
It has only command line programs. It is in the folder example2. Read
the README file there.

Deleting shared memory, made specifically for the testing programs,
is with example_shmrm (for example) example1_shmrm (for example1) and
example2_shmrm (for example2). They deletes shared memory segments for 
testing programs respectively and their special codes are defined in 
conf_lib.h.

A detailed description of the constants and the ways of their calculation 
can be read in ./docs/maccess_cons_types_vars.txt. Detailed description 
of all functions of the maccess package is in 
./docs/maccess_functions.txt.

The theory of the binary tree and the version B+ (the inventors are 
Rudolf Bayer and Edward Meyers McCreight) which is applied in this 
package, can be read in the B+tree_theory.txt file.

--------------------------------------------------------------------------
NOTES: 

UI libraries (scr*.c & string.c, time.c, date.c) are made to work with 
garbage collector (we use The Boehm–Demers–Weiser garbage collector).
On the other hand, database library (ma*.c) does NOT depend on garbage 
collector, but of course, can be used with it as additional security 
against potential memory leak. Usage of garbage collector is defined 
as a macro in conf_lib.h

scr*.c, example.c, example1.c requires ncurses libraries, but command line
programs can work without it. They can be compiled with:  
make -f Makefile_only_command_line

Program for testing key codes and example how to function keys, ESC and 
the like are called key.
