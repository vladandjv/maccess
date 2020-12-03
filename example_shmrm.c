/* example_shmrm.c - Removing shared memory segment for example */

#include "defs.h"
#include "conf_lib.h"

#define APP_LOGFILE "example.log"
int
main ()
{

  openLog (APP_LOGFILE); /* Always first. */

  Destroy_SHM (LOCK_SHM_MEM_CODE, (size_t) LOCK_SHM_MEM_SIZE);
  Destroy_SHM (MACCESS_SHM_MEM_CODE, (size_t) MACCESS_SHM_MEM_SIZE);

  printf ("Shared memory segemnts for example are deleted!\n");
  logMessage ("Shared memory segemnts for example are deleted!");


  closeLog ();

  return 0;
}


