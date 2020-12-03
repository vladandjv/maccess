/**************************************************************************/
/* PROGRAM: E2_ORIGIN_PRINT.C                                             */
/* DESCRIPTION: Example program for Maccess funcitons.                    */
/* AUTHOR: VLADAN DJORDJEVIC                                               */
/**************************************************************************/
#include "maccess.h"
#include "example2.h"

#define APP_LOGFILE "example2.log"

FileName DatFName = "origin.dbc";
FileName DatFName1 = "origin.tmp";
FileName IndexFName0 = "origin_record_id.cdx";
FileName IndexFName1 = "origin_origin_name.cdx";
struct Record_origin *info;
unsigned long long origin_record_id_Len = 65; /* Should be key lenght + 1 */
unsigned long long origin_origin_name_Len = 65; /* Should be key lenght + 1 */
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
      CloseFile (DPtr_origin, &DExt_origin);
      CloseIndex (IPtr_origin_record_id, &IExt_origin_record_id);
      CloseIndex (IPtr_origin_origin_name, &IExt_origin_origin_name);
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
  OpenFile (&DPtr_origin, &DExt_origin, DatFName, sizeof (struct Record_origin), (long long) 0, (long long) 0);
  OpenIndex (&IPtr_origin_record_id, &IExt_origin_record_id, IndexFName0, origin_record_id_Len, NoDuplicates, (long long) 0, (long long) 1);
  OpenIndex (&IPtr_origin_origin_name, &IExt_origin_origin_name, IndexFName1, origin_origin_name_Len, NoDuplicates, (long long) 0, (long long) 2);
  SHM_UnLock (Lock);

  printf ("I am working!\n");
  Next ();
  printf ("DONE!\n");

  SHM_Lock (Lock);
  CloseFile (DPtr_origin, &DExt_origin);
  CloseIndex (IPtr_origin_record_id, &IExt_origin_record_id);
  CloseIndex (IPtr_origin_origin_name, &IExt_origin_origin_name);
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
  char Mat[140];
  char buffer[2];
  long long i;
  info = &R_origin;

  for (i = 1;; i++)
    {
      SHM_Lock (Lock);
      NextKey (IPtr_origin_record_id, &IExt_origin_record_id, &TaRecNum, (TaKeyStr *) temp);
      if (OKAY == T)
        {
          GetRec (DPtr_origin, &DExt_origin, TaRecNum, info);
          SHM_UnLock (Lock);
#ifdef DEBUG_APP
          logMessage ("P1 Iteration= %lld", i);
          logMessage ("P1 info->Deleted = %lld", info->Deleted);
          logMessage ("P1 record_id = [%s] %d", info->record_id, strlen (info->record_id));
          logMessage ("P1 origin_name = [%s] %d", info->origin_name, strlen (info->origin_name));
          logMessage ("P1 allowed = [%d]", info->allowed);
#endif
          strcpy (Mat, info->record_id);
          strcat (Mat, " ");
          strcat (Mat, info->origin_name);
          strcat (Mat, " ");
          sprintf (buffer, "%01d", info->allowed);
          strcat (Mat, buffer);
          printf ("Mat[%10lld] = [%s]\n", i, Mat);
#ifdef DEBUG_APP
          logMessage ("Recored print lenght = %ld", strlen (Mat));
#endif
        }
      else
        {
          SHM_UnLock (Lock);
          if (i == 1)
            printf ("The database is empty! Load it with ./e2_origin_load\n");
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
      printf ("There are no files to print! Use e2_origin_load first.\n");
      exit (1);
    }
}
/**************************************************************************/

