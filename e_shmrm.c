/**************************************************************************/
/* PROGRAM: E_SHMRM.C                                                     */
/* DESCRIPTION: Deletes shared memory segment created by example programs */
/* and removes the locking semaphore.                                     */
/* NOTE: The shared memory segment must not be attached by some process,  */
/* otherwise kernel refueses to delete it. The same is if it is created   */
/* by another user, and you are not root user.                            */
/* AUTHOR: VLADAN DJORDJEVIC                                              */
/**************************************************************************/

#include "defs.h"
#include "maccess.h"

int main()
{

   openLog(APP_LOGFILE); /* Always first. */
#ifndef SINGLE_USER_NO_SHARED_MEMORY
   if (Destroy_SHM(MACCESS_SHM_MEM_CODE, (size_t)MACCESS_SHM_MEM_SIZE) == -1)
   {
      printf("Cannot destroy the maccess shared memory segment!\n");
      logMessage("Cannot destroy the maccess shared memory segment!");
   }
   else
   {
      printf("The shared memory segemnts for example is deleted!\n");
      logMessage("The shared memory segemnts for example is deleted!");
   }

   if(Destroy_Semaphore() == -1)
   {
      printf("Cannot destroy the maccess semaphore!\n");
      logMessage("Cannot destroy the maccess semaphore!");
   }
   else
   {
      printf("The semaphore is removed!\n");
      logMessage("The semaphore is removed!");
   }

#else
   printf("Single user mode! The shared memory segemnt do not exist!\n");
   logMessage("Single user mode! The shared memory segemnt do not exist!");
#endif

   closeLog();

   return 0;
}
