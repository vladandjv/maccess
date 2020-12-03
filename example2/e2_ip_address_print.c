/**************************************************************************/
/* PROGRAM: E2_IP_ADDRESS_PRINT.C                                         */
/* DESCRIPTION: Example program for Maccess funcitons.                    */
/* AUTHOR: VLADAN DJORDJEVIC                                               */
/**************************************************************************/
#include "maccess.h"
#include "example2.h"

#define APP_LOGFILE "example2.log"

FileName DatFName = "ip_address.dbc";
FileName DatFName1 = "ip_address.tmp";
FileName IndexFName0 = "ip_address_record_id.cdx";
FileName IndexFName1 = "ip_address_ip4_add.cdx";
FileName IndexFName2 = "ip_address_ip_allowed.cdx";
struct Record_ip_address *info;
unsigned long long record_id_Len = 65; /* Should be key lenght + 1 */
unsigned long long ip4_add_Len = 17; /* Should be key lenght + 1 */
unsigned long long allowed_Len = 2; /* Should be key lenght + 1 */
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
      CloseFile (DPtr_ip_address, &DExt_ip_address);
      CloseIndex (IPtr_ip_address_record_id, &IExt_ip_address_record_id);
      CloseIndex (IPtr_ip_address_ip4_add, &IExt_ip_address_ip4_add);
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
  OpenFile (&DPtr_ip_address, &DExt_ip_address, DatFName, sizeof (struct Record_ip_address), (long long) 0, (long long) 0);
  OpenIndex (&IPtr_ip_address_record_id, &IExt_ip_address_record_id, IndexFName0, record_id_Len, NoDuplicates, (long long) 0, (long long) 1);
  OpenIndex (&IPtr_ip_address_ip4_add, &IExt_ip_address_ip4_add, IndexFName1, ip4_add_Len, Duplicates, (long long) 0, (long long) 2); /* NoDuplicates for the server */
  OpenIndex (&IPtr_ip_address_allowed, &IExt_ip_address_allowed, IndexFName2, allowed_Len, Duplicates, (long long) 0, (long long) 3);
  SHM_UnLock (Lock);

  printf ("I am working!\n");
  Next ();
  printf ("DONE!\n");

  SHM_Lock (Lock);
  CloseFile (DPtr_ip_address, &DExt_ip_address);
  CloseIndex (IPtr_ip_address_record_id, &IExt_ip_address_record_id);
  CloseIndex (IPtr_ip_address_ip4_add, &IExt_ip_address_ip4_add);
  CloseIndex (IPtr_ip_address_allowed, &IExt_ip_address_allowed);
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
  char Mat[85];
  long long i;
  info = &R_ip_address;

  for (i = 1;; i++)
    {
      SHM_Lock (Lock);
      NextKey (IPtr_ip_address_record_id, &IExt_ip_address_record_id, &TaRecNum, (TaKeyStr *) temp);
      //NextKey(IPtr_ip_address_ip4_add, &IExt_ip_address_ip4_add, &TaRecNum, (TaKeyStr *) temp);
      if (OKAY == T)
        {
          GetRec (DPtr_ip_address, &DExt_ip_address, TaRecNum, info);
          SHM_UnLock (Lock);
#ifdef DEBUG_APP
          logMessage ("P2 Iteration= %ld", i);
          logMessage ("P2 info->Deleted = %lld", info->Deleted);
          logMessage ("P2 record_id = [%s] %d", info->record_id, strlen (info->record_id));
          logMessage ("P2 ip4_add = [%s] %d", info->ip4_add, strlen (info->ip4_add));
          logMessage ("P2 allowed = [%s] %d", info->allowed, strlen (info->allowed));
#endif
          strcpy (Mat, info->record_id);
          printf ("[%s]\n", Mat);
          strcat (Mat, " ");
          strcat (Mat, info->ip4_add);
          strcat (Mat, " ");
          strcat (Mat, info->allowed);
          printf ("Mat[%10lld] = [%s]\n", i, Mat);
#ifdef DEBUG_APP
          logMessage ("Recored print lenght = %ld", strlen (Mat));
#endif
        }
      else
        {
          SHM_UnLock (Lock);
          if (i == 1)
            printf ("The database is empty! Load it with ./e2_ip_address_load\n");
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
      printf ("There are no files to print! Use e2_ip_address_load first.\n");
      exit (1);
    }
}
/**************************************************************************/

