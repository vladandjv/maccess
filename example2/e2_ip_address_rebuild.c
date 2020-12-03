/**************************************************************************/
/* PROGRAM: E2_IP_ADDRESS_RUBUILD.C                                       */
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
unsigned long long record_id_Len = 65; /* Should be key lenght + 1 */
unsigned long long ip4_add_Len = 17; /* Should be key lenght + 1 */
unsigned long long allowed_Len = 2; /* Should be key lenght + 1 */
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
  void RebuildIndex (), Control ();

  openLog (APP_LOGFILE);
  signal (SIGSEGV, sig_handler);
  signal (SIGINT, sig_handler);
  if (signal (SIGTERM, sig_handler) == SIG_ERR)
    logMessage ("Can't catch SIGTERM signal");
  Control ();

  printf ("I am working!\n");
  RebuildIndex ();
  printf ("DONE!\n");

  closeLog ();
  exit (0);
}
/**************************************************************************/
void
RebuildIndex ()
{
  long long RecordNumber, RecNum, velic;
  struct Record_ip_address CurRec, CurRec1;
  struct DataExt DExt1;
  DataFilePtr DPtr1;

  Destroy_SHM (MACCESS_SHM_MEM_CODE_2, (size_t) MACCESS_SHM_MEM_SIZE);
  InitAccess ((long long) 0, MACCESS_SHM_MEM_CODE_2);
  MakeFile (&DPtr1, &DExt1, DatFName1, sizeof (struct Record_ip_address), (long long) 0, (long long) 0);
  CloseFile (DPtr1, &DExt1);
  TermAccess ((long long) 0);
  Destroy_SHM (MACCESS_SHM_MEM_CODE_2, (size_t) MACCESS_SHM_MEM_SIZE);

  InitAccess ((long long) 0, MACCESS_SHM_MEM_CODE_2);
  OpenFile (&DPtr1, &DExt1, DatFName1, sizeof (struct Record_ip_address), (long long) 0, (long long) 0);
  OpenFile (&DPtr_ip_address, &DExt_ip_address, DatFName, sizeof (struct Record_ip_address), (long long) 0, (long long) 1);
  velic = FileLen (DPtr_ip_address, &DExt_ip_address) - 1;
  for (RecordNumber = 1; RecordNumber <= velic; RecordNumber++)
    {
      GetRec (DPtr_ip_address, &DExt_ip_address, RecordNumber, &CurRec);
      if (CurRec.Deleted == 0)
        {
          CurRec1.Deleted = CurRec.Deleted;
          strcpy (CurRec1.record_id, CurRec.record_id);
          strcpy (CurRec1.ip4_add, CurRec.ip4_add);
          strcpy (CurRec1.allowed, CurRec.allowed);
          AddRec (DPtr1, &DExt1, &RecNum, &CurRec1);
        }
    }
  CloseFile (DPtr_ip_address, &DExt_ip_address);
  CloseFile (DPtr1, &DExt1);
  TermAccess ((long long) 0);
  Destroy_SHM (MACCESS_SHM_MEM_CODE_2, (size_t) MACCESS_SHM_MEM_SIZE);

  system ("mv ip_address.tmp ip_address.dbc");
  system ("rm -rf ip_address_record_id.cdx ip_address_ip4_add.cdx \
		    ip_address_ip_allowed.cdx");

  InitAccess ((long long) 0, MACCESS_SHM_MEM_CODE_2);
  MakeIndex (&IPtr_ip_address_record_id, &IExt_ip_address_record_id, IndexFName0, record_id_Len, NoDuplicates, (long long) 0, (long long) 0);
  CloseIndex (IPtr_ip_address_record_id, &IExt_ip_address_record_id);
  MakeIndex (&IPtr_ip_address_ip4_add, &IExt_ip_address_ip4_add, IndexFName1, ip4_add_Len, NoDuplicates, (long long) 0, (long long) 1);
  CloseIndex (IPtr_ip_address_ip4_add, &IExt_ip_address_ip4_add);
  MakeIndex (&IPtr_ip_address_allowed, &IExt_ip_address_allowed, IndexFName2, allowed_Len, Duplicates, (long long) 0, (long long) 3);
  CloseIndex (IPtr_ip_address_allowed, &IExt_ip_address_allowed);
  TermAccess ((long long) 0);
  Destroy_SHM (MACCESS_SHM_MEM_CODE_2, (size_t) MACCESS_SHM_MEM_SIZE);

  InitAccess ((long long) 0, MACCESS_SHM_MEM_CODE_2);
  OpenFile (&DPtr_ip_address, &DExt_ip_address, DatFName, sizeof (struct Record_ip_address), (long long) 0, (long long) 0);
  OpenIndex (&IPtr_ip_address_record_id, &IExt_ip_address_record_id, IndexFName0, record_id_Len, NoDuplicates, (long long) 0, (long long) 1);
  OpenIndex (&IPtr_ip_address_ip4_add, &IExt_ip_address_ip4_add, IndexFName1, ip4_add_Len, Duplicates, (long long) 0, (long long) 2);
  OpenIndex (&IPtr_ip_address_allowed, &IExt_ip_address_allowed, IndexFName2, allowed_Len, Duplicates, (long long) 0, (long long) 3);
  velic = FileLen (DPtr_ip_address, &DExt_ip_address) - 1;
  for (RecordNumber = 1; RecordNumber <= velic; RecordNumber++)
    {
      GetRec (DPtr_ip_address, &DExt_ip_address, RecordNumber, &CurRec);
      if (CurRec.Deleted == 0)
        {
          AddKey (IPtr_ip_address_record_id, &IExt_ip_address_record_id, &RecordNumber, (TaKeyStr *) CurRec.record_id);
          AddKey (IPtr_ip_address_ip4_add, &IExt_ip_address_ip4_add, &RecordNumber, (TaKeyStr *) CurRec.ip4_add);
          AddKey (IPtr_ip_address_allowed, &IExt_ip_address_allowed, &TaRecNum, (TaKeyStr *) CurRec.allowed);
        }
    }
  CloseFile (DPtr_ip_address, &DExt_ip_address);
  CloseIndex (IPtr_ip_address_record_id, &IExt_ip_address_record_id);
  CloseIndex (IPtr_ip_address_ip4_add, &IExt_ip_address_ip4_add);
  CloseIndex (IPtr_ip_address_allowed, &IExt_ip_address_allowed);
  TermAccess ((long long) 0);
  Destroy_SHM (MACCESS_SHM_MEM_CODE_2, (size_t) MACCESS_SHM_MEM_SIZE);
}
/**************************************************************************/
void
Control ()
{
  if (fopen (DatFName, "rb") == NULL)
    {
      printf ("There are no files to rebuild! Use e2_ip_address_load.\n");
      exit (1);
    }
}
/**************************************************************************/

