/* example_shmrm.c - Removing shared memory segment */

#include "defs.h"
#include "maccess.h"

#define APP_LOGFILE "example.log"
int
main ()
{

  openLog (APP_LOGFILE); /* Always first. */

  if (Destroy_SHM (LOCK_SHM_MEM_CODE, (size_t) LOCK_SHM_MEM_SIZE) == -1)
     {
	 printf("Cannot destroy the lock shared memory segment!\n"); 
	 logMessage("Cannot destroy the lock shared memory segment!");
     }
  else if (Destroy_SHM (MACCESS_SHM_MEM_CODE, (size_t) MACCESS_SHM_MEM_SIZE) == -1)
     {
	printf("Cannot destroy the maccess shared memory segment!\n"); 
	logMessage("Cannot destroy the maccess shared memory segment!");
     }
  else
  {
        printf ("The shared memory segemnts for example are deleted!\n");
        logMessage ("The shared memory segemnts for example are deleted!");
  }


  closeLog ();

  return 0;
}


