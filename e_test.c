/*
 * PROGRAM: E_TEST.C                                                        
 * DESCRIPTION: Example program for Maccess funcitons, without any other
 * libraries. It loads only 5 records in the same files with which the 
 * example program works. Use in conjunction with the example and e_print
 * to ses results.
 * AUTHOR: VLADAN DJORDJEVIC                                               
 */
#include "maccess.h"
#include "example.h"
#include <stdint.h>

FileName DatFName = "data.dbc";
FileName IndexFName = "data.cdx";
unsigned long long KeyLen = 11; /* Should be key lenght + 1 */
long long *Lock;                /* locks the shared memory segment*/
struct IndexExt IExt;
struct DataExt DExt;
DataFilePtr IPtr = NULL;
struct Record OurRecord;
struct Record *info = NULL;
DataFilePtr DPtr = NULL;
long long TaRecNum = 0;

int main()
{
  void Control(), Loading();

  printf("MaxKeyLen       = %d\n", MaxKeyLen);
  printf("PageSize        = %d\n", PageSize);
  printf("PageStackSize   = %d\n", PageStackSize);
  printf("Order           = %lld\n", Order);
  printf("MaxHeight       = %d\n", MaxHeight);
  printf("FileHeaderSize = %zu\n", FileHeaderSize);
  printf("MinDataRecSize = %zu\n", MinDataRecSize);
  printf("ItemOverhead   = %zu\n", ItemOverhead);
  printf("PageOverhead   = %zu\n", PageOverhead);
  printf("MaxDataRecSize = %d\n", MaxDataRecSize);
  printf("INT_MAX = %d\n", INT_MAX);
  printf("INT32_MAX = %d\n", INT32_MAX);
  printf("INT64_MAX = %ld\n", INT64_MAX);
  printf("LONG_MAX = %ld\n", LONG_MAX);
  printf("LLONG_MAX = %lld\n", LLONG_MAX);
  printf("Press any key to continue!\n");

  Control();
  Lock = DB_Lock_Init(LOCK_SHM_MEM_CODE, LOCK_SHM_MEM_SIZE);
  SHM_Lock(Lock);
  InitAccess(MACCESS_SHM_MEM_CODE);
  OpenFile(&DPtr, &DExt, DatFName, sizeof(struct Record), (long long)0);
  OpenIndex(&IPtr, &IExt, IndexFName, KeyLen, Duplicates, (long long)1);
  Loading();
  SHM_UnLock(Lock);
  printf("DONE!\n");

  CloseFile(DPtr, &DExt);
  CloseIndex(IPtr, &IExt);
  TermAccess();
  SHM_UnLock(Lock);
  DB_Lock_Close(Lock);
  exit(0);
}
/**************************************************************************/
void Loading()
{
  void Control();
  long long i;
  info = &OurRecord;

  info->Deleted = 0;
  for (i = 1; i <= 10; i++)
  {
    sprintf(info->Key, "%010lld", i);
    sprintf(info->Surname, "%025lld", i);
    sprintf(info->Name, "%020lld", i);
    sprintf(info->Remark, "%045lld", i);
    SHM_Lock(Lock);
    AddRec(DPtr, &DExt, &TaRecNum, info);
    AddKey(IPtr, &IExt, &TaRecNum, (TaKeyStr *)info->Key);
    SHM_UnLock(Lock);
    printf("%07lld - Key=[%s] Surname=[%s] Key_Len=%zd\n", i, info->Key,
           info->Surname, strlen(info->Key));
  }
}
/**************************************************************************/
void Control()
{
  if (fopen(DatFName, "rb") == NULL)
  {
    printf("There are no files! Use e_create first.\n");
    exit(1);
  }
}
/**************************************************************************/
