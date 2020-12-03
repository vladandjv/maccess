/*
 * PROGRAM: E_LOAD.C                                                        
 * DESCRIPTION: Example program for Maccess funcitons, without any other
 * libraries. It loads 10000 records in the same files with which the 
 * example program works. Use in conjunction with the example to ses results.
 * AUTHOR: VLADAN DJORDJEVIC                                               
 */

#include "maccess.h"
#include "example.h"

#define APP_LOGFILE "example.log"

FileName DatFName = "data.dbc";
FileName IndexFName = "data.cdx";
unsigned long long KeyLen = 11; /* Should be key lenght + 1 */
long long *Lock; /* locks the shared memory segment*/
struct IndexExt IExt;
struct DataExt DExt;
DataFilePtr IPtr = NULL;
struct Record OurRecord;
struct Record *info = NULL;
DataFilePtr DPtr = NULL;
long long TaRecNum = 0;

void prepare_str (char *str, int length);
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
      CloseFile (DPtr, &DExt);
      CloseIndex (IPtr, &IExt);
      SHM_UnLock (Lock);
      TermAccess ((long long) 0);
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
  void Control (), Loading ();

  openLog (APP_LOGFILE);
  signal (SIGSEGV, sig_handler);
  signal (SIGINT, sig_handler);
  if (signal (SIGTERM, sig_handler) == SIG_ERR)
    logMessage ("Can't catch SIGTERM signal");

  Lock = DB_Lock_Init (LOCK_SHM_MEM_CODE, LOCK_SHM_MEM_SIZE);
  SHM_Lock (Lock);
  Control (); /* Must be before InitAccess */
  InitAccess ((long long) 0, MACCESS_SHM_MEM_CODE);
  OpenFile (&DPtr, &DExt, DatFName, sizeof (struct Record), (long long) 0, (long long) 0);
  OpenIndex (&IPtr, &IExt, IndexFName, KeyLen, Duplicates, (long long) 0, (long long) 1);
  SHM_UnLock (Lock);
  ClearKey (&IExt);

  printf ("I am working!\n");
  Loading ();
  printf ("DONE!\n");

  SHM_Lock (Lock);
  CloseFile (DPtr, &DExt);
  CloseIndex (IPtr, &IExt);
  TermAccess ((long long) 0);
  SHM_UnLock (Lock);
  DB_Lock_Close (Lock);
  closeLog ();
  exit (0);
}
/**************************************************************************/
void
Loading ()
{
  long long i;
  info = &OurRecord;

  logMessage ("I am loading 100000 records");
  info->Deleted = 0;
  //for (i = 1; i <= 10000000; i++) {
  for (i = 1; i <= 1000000; i++)
    {
      sprintf (info->Key, "%010lld", i);
      prepare_str (info->Key, 10);
      sprintf (info->Surname, "%025lld", i);
      sprintf (info->Name, "%020lld", i);
      sprintf (info->Remark, "%045lld", i);
      SHM_Lock (Lock);
      AddRec (DPtr, &DExt, &TaRecNum, info);
      AddKey (IPtr, &IExt, &TaRecNum, (TaKeyStr *) info->Key);
      SHM_UnLock (Lock);
#ifdef DEBUG_APP
      logMessage ("Key[%s] Surname[%s] Name[%s] Remark[%s]",
                  info->Key, info->Surname, info->Name, info->Remark);
#endif 
    }
  logMessage ("DONE! I have loaded 10000 records");
}
/**************************************************************************/
void
Control ()
{
  FILE *fp = NULL;

  if ((fp = fopen (DatFName, "OrdNum")) == NULL)
    {
      Destroy_SHM (MACCESS_SHM_MEM_CODE, (size_t) MACCESS_SHM_MEM_SIZE);
      InitAccess ((long long) 0, MACCESS_SHM_MEM_CODE);
      MakeFile (&DPtr, &DExt, DatFName, sizeof (struct Record), (long long) 0, (long long) 0);
      MakeIndex (&IPtr, &IExt, IndexFName, KeyLen, NoDuplicates, (long long) 0, (long long) 1);
      CloseFile (DPtr, &DExt);
      CloseIndex (IPtr, &IExt);
      TermAccess ((long long) 0);
      Destroy_SHM (MACCESS_SHM_MEM_CODE, (size_t) MACCESS_SHM_MEM_SIZE);
    }
  else
    fclose (fp);
}
/**************************************************************************/
/* Remove empty strings and add ' ' at the end if string lenght is less
 * then required */
void
prepare_str (char *str, int length)
{
  char blank[MAX_STRING_LENGTH];
  int c, d;

  for (c = 0, d = 0; str[c] != '\0'; c++)
    {
      if (str[c] != ' ')
        {
          blank[d] = str[c];
          break;
        }
    }

  for (; str[c] != '\0'; c++, d++)
    blank[d] = str[c];

  for (; d <= length; d++)
    {
      blank[d] = ' ';
    }
  blank[length] = '\0';
  strcpy (str, blank);

  return;
}
/**************************************************************************/

