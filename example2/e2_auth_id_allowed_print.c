/**************************************************************************/
/* PROGRAM: E2_AUTH_ID_ALLOWED_PRINT.C                                    */
/* DESCRIPTION: Example program for Maccess funcitons.                    */
/* AUTHOR: VLADAN DJORDJEVIC                                               */
/**************************************************************************/
#include "maccess.h"
#include "example2.h"

#define APP_LOGFILE "example2.log"

FileName DatFName = "auth_id_allowed.dbc";
FileName DatFName1 = "auth_id_allowed.tmp";
FileName IndexFName0 = "auth_id_allowed_record_id.cdx";
FileName IndexFName1 = "auth_id_allowed_auth_id.cdx";
struct Record_auth_id_allowed *info;
unsigned long long auth_id_allowed_record_id_Len = 65; /* Key lenght + 1 */
unsigned long long auth_id_allowed_auth_id_Len = 65; /* Key lenght + 1 */
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
      CloseFile (DPtr_auth_id_allowed, &DExt_auth_id_allowed);
      CloseIndex (IPtr_auth_id_allowed_record_id, &IExt_auth_id_allowed_record_id);
      CloseIndex (IPtr_auth_id_allowed_auth_id, &IExt_auth_id_allowed_auth_id);
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
  OpenFile (&DPtr_auth_id_allowed, &DExt_auth_id_allowed, DatFName, sizeof (struct Record_auth_id_allowed), (long long) 0, (long long) 0);
  OpenIndex (&IPtr_auth_id_allowed_record_id, &IExt_auth_id_allowed_record_id, IndexFName0, auth_id_allowed_record_id_Len, NoDuplicates, (long long) 0, (long long) 1);
  OpenIndex (&IPtr_auth_id_allowed_auth_id, &IExt_auth_id_allowed_auth_id, IndexFName1, auth_id_allowed_auth_id_Len, NoDuplicates, (long long) 0, (long long) 2);
  SHM_UnLock (Lock);

  printf ("I am working!\n");
  Next ();
  printf ("DONE!\n");

  SHM_Lock (Lock);
  CloseFile (DPtr_auth_id_allowed, &DExt_auth_id_allowed);
  CloseIndex (IPtr_auth_id_allowed_record_id, &IExt_auth_id_allowed_record_id);
  CloseIndex (IPtr_auth_id_allowed_auth_id, &IExt_auth_id_allowed_auth_id);
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
  long long i;
  info = &R_auth_id_allowed;

  for (i = 1;; i++)
    {
      SHM_Lock (Lock);
      NextKey (IPtr_auth_id_allowed_record_id, &IExt_auth_id_allowed_record_id, &TaRecNum, (TaKeyStr *) temp);
      if (OKAY == T)
        {
          GetRec (DPtr_auth_id_allowed, &DExt_auth_id_allowed, TaRecNum, info);
          SHM_UnLock (Lock);
#ifdef DEBUG_APP
          logMessage ("P1 info->Deleted = %lld", info->Deleted);
          logMessage ("P3 Iteration= %ld", i);
          logMessage ("P3 info->Deleted = %lld", info->Deleted);
          logMessage ("P3 record_id = [%s] %d", info->record_id, strlen (info->record_id));
          logMessage ("P3 auth_id = [%s] %d", info->auth_id, strlen (info->auth_id));
#endif

          strcpy (Mat, info->record_id);
          strcat (Mat, " ");
          strcat (Mat, info->auth_id);
          strcat (Mat, " ");
          printf ("Mat[%10lld] = [%s]\n", i, Mat);
#ifdef DEBUG_APP
          logMessage ("Recored print lenght = %ld", strlen (Mat));
#endif
        }
      else
        {
          SHM_UnLock (Lock);
          if (i == 1)
            printf ("The database is empty! Load it with \
			    ./e2__auth_id_allowed_load.\n");
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
      printf ("There are no files to print! Use \
		      e2_atuh_id_allowed_load first.\n");
      exit (1);
    }
}
/**************************************************************************/

