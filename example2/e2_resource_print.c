/**************************************************************************/
/* PROGRAM: E2_RESOURCE_PRINT.C                                           */
/* DESCRIPTION: Example program for Maccess funcitons.                    */
/* AUTHOR: VLADAN DJORDJEVIC                                               */
/**************************************************************************/
#include "maccess.h"
#include "example2.h"

#define APP_LOGFILE "example2.log"

FileName DatFName = "resource.dbc";
FileName DatFName1 = "resource.tmp";
FileName IndexFName0 = "resource_record_id.cdx";
FileName IndexFName1 = "resource_resource_id.cdx";
struct Record_resource *info;
unsigned long long resource_record_id_Len = 65; /* Key lenght + 1 */
unsigned long long resource_resource_id_Len = 32; /* Key lenght + 1 */
long long *Lock; /* locks the shared memory segment*/
/**************************************************************************/
void
sig_handler (int sig)
{

#ifdef DEBUG_APP
  logMessage ("Cought signal %d in sigint_handler", sig);
#endif

  switch (sig)
    {
    case SIGTERM:
    case SIGSEGV:
      SHM_UnLock (Lock);
      CloseFile (DPtr_resource, &DExt_resource);
      CloseIndex (IPtr_resource_record_id, &IExt_resource_record_id);
      CloseIndex (IPtr_resource_resource_id, &IExt_resource_resource_id);
      SHM_UnLock (Lock);
      TermAccess ((long long) 0);
      SHM_UnLock (Lock);
      DB_Lock_Close (Lock);
      closeLog ();
      exit (0);
    case SIGINT:
      signal (sig, SIG_IGN);
      break;

    default:
      signal (sig, SIG_DFL);
    }
}
/**************************************************************************/
int
main ()
{
  void Next (), Control ();

  openLog (APP_LOGFILE);
  signal (SIGSEGV, sig_handler);
  signal (SIGINT, sig_handler);
  if (signal (SIGTERM, sig_handler) == SIG_ERR)
    logMessage ("Can't catch SIGTERM signal");

  Lock = DB_Lock_Init (LOCK_SHM_MEM_CODE_2, LOCK_SHM_MEM_SIZE);
  SHM_Lock (Lock);
  Control (); /* Must be before InitAccess */
  InitAccess ((long long) 0, MACCESS_SHM_MEM_CODE_2);
  OpenFile (&DPtr_resource, &DExt_resource, DatFName, sizeof (struct Record_resource), (long long) 0, (long long) 0);
  OpenIndex (&IPtr_resource_record_id, &IExt_resource_record_id, IndexFName0, resource_record_id_Len, NoDuplicates, (long long) 0, (long long) 1);
  OpenIndex (&IPtr_resource_resource_id, &IExt_resource_resource_id, IndexFName1, resource_resource_id_Len, NoDuplicates, (long long) 0, (long long) 2);
  SHM_UnLock (Lock);

  printf ("I am working!\n");
  Next ();
  printf ("DONE!\n");

  SHM_Lock (Lock);
  CloseFile (DPtr_resource, &DExt_resource);
  CloseIndex (IPtr_resource_record_id, &IExt_resource_record_id);
  CloseIndex (IPtr_resource_resource_id, &IExt_resource_resource_id);
  TermAccess ((long long) 0);
  SHM_UnLock (Lock);
  DB_Lock_Close (Lock);
  closeLog ();
  exit (0);
}
/**************************************************************************/
void
Next ()
{
  char temp[65];
  char Mat[420];
  long long i;
  info = &R_resource;

  for (i = 1;; i++)
    {
      SHM_Lock (Lock);
      NextKey (IPtr_resource_record_id, &IExt_resource_record_id, &TaRecNum, (TaKeyStr *) temp);
      if (OKAY == T)
        {
          GetRec (DPtr_resource, &DExt_resource, TaRecNum, info);
          SHM_UnLock (Lock);
#ifdef DEBUG_APP
          logMessage ("P4 Iteration= %lld", i);
          logMessage ("P4 info->Deleted = %lld", info->Deleted);
          logMessage ("P4 record_id = [%s] %d", info->record_id, strlen (info->record_id));
          logMessage ("P4 origin_name = [%s] %d", info->resource_id, strlen (info->resource_id));
          logMessage ("P4 bash_script = [%s] %d", info->bash_script, strlen (info->bash_script));
          logMessage ("P4 special_use = [%s] %d", info->special_use, strlen (info->special_use));
#endif
          strcpy (Mat, info->record_id);
          strcat (Mat, " ");
          strcat (Mat, info->resource_id);
          strcat (Mat, " ");
          strcat (Mat, info->bash_script);
          strcat (Mat, " ");
          strcat (Mat, info->special_use);
          printf ("Mat[%10lld] = [%s]\n", i, Mat);
#ifdef DEBUG_APP
          logMessage ("Recored print lenght = %ld", strlen (Mat));
#endif
        }
      else
        {
          SHM_UnLock (Lock);
          if (i == 1)
            printf ("The database is empty! Load it with ./e2_resource_load\n");
          break;
        }
    }
}
/**************************************************************************/
void
Control ()
{
  if (fopen (DatFName, "rb") == NULL)
    {
      SHM_UnLock (Lock);
      printf ("There are no files to print! Use e2_resource_load first.\n");
      exit (1);
    }
}
/****************************** END ***************************************/
