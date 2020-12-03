/* example2_shmrm.c - Removing shared memory segments for example2 */

#include "defs.h"
#include "conf_lib.h"

#define APP_LOGFILE "example2.log"
int
main ()
{

  openLog (APP_LOGFILE); /* Always first. */

  Destroy_SHM (LOCK_SHM_MEM_CODE_2, (size_t) LOCK_SHM_MEM_SIZE);
  Destroy_SHM (MACCESS_SHM_MEM_CODE_2, (size_t) MACCESS_SHM_MEM_SIZE);

  printf ("Shared memory segemnts for example2 are deleted!\n");
  logMessage ("Shared memory segemnts for example2 are deleted!");

  closeLog ();

  return 0;
}


