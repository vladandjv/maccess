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

#define APP_LOGFILE "example.log"

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

void prepare_str(char *str, int length);
int main()
{
  void Control(), Loading();

  printf("The singl task version! Just to check parameters and memory.\n");
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
  getchar();

  Control();
  InitAccess(MACCESS_SHM_MEM_CODE);
  OpenFile(&DPtr, &DExt, DatFName, sizeof(struct Record), (long long)0);
  OpenIndex(&IPtr, &IExt, IndexFName, KeyLen, Duplicates, (long long)1);
  ClearKey(&IExt);
  Control();
  Loading();
  printf("DONE!\n");

  CloseFile(DPtr, &DExt);
  CloseIndex(IPtr, &IExt);
  TermAccess();
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
    prepare_str(info->Key, 10);
    sprintf(info->Surname, "%025lld", i);
    sprintf(info->Name, "%020lld", i);
    sprintf(info->Remark, "%045lld", i);
    AddRec(DPtr, &DExt, &TaRecNum, info);
    AddKey(IPtr, &IExt, &TaRecNum, (TaKeyStr *)info->Key);
    printf("%07lld - Key=[%s] Surname=[%s] Key_Len=%zd\n", i, info->Key,
           info->Surname, strlen(info->Key));
  }
}
/**************************************************************************/
/* Remove empty strings and add ' ' at the end if string lenght is less
 * then required */
void prepare_str(char *str, int length)
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
  strcpy(str, blank);

  return;
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