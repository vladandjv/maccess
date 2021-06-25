/*
 * PROGRAM: E_LOAD.C                                                        
 * DESCRIPTION: Only creates the database files.
 * AUTHOR: VLADAN DJORDJEVIC                                               
 */

#include "maccess.h"
#include "example.h"

FileName DatFName = "data.dbc";
FileName IndexFName = "data.cdx";
unsigned long long KeyLen = 11; /* Should be key lenght + 1 */
struct IndexExt IExt;
struct DataExt DExt;
DataFilePtr IPtr = NULL;
struct Record OurRecord;
struct Record *info = NULL;
DataFilePtr DPtr = NULL;

/**************************************************************************/
int main()
{
  void Loading();

  InitAccess(MACCESS_SHM_MEM_CODE);
  MakeFile(&DPtr, &DExt, DatFName, sizeof(struct Record), (long long)0);
  MakeIndex(&IPtr, &IExt, IndexFName, KeyLen, NoDuplicates, (long long)1);
  CloseFile(DPtr, &DExt);
  CloseIndex(IPtr, &IExt);
  TermAccess();
  exit(0);
}
/**************************************************************************/
