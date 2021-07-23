/**************************************************************************/
/* PROGRAM: MAMAKE.C                                                      */
/* DESCRIPTION:  Creates data and index files                             */
/* AUTHOR: VLADAN DJORDJEVIC                                              */
/**************************************************************************/

#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include "maccess.h"
/**************************************************************************/
void
    WriteHeader(DatF, DatE, RecLen)

        struct DataFile *DatF;
struct DataExt *DatE;
long long RecLen;
{
  unsigned long long n;
  unsigned long long R = 0;

  for (n = 0; n < MaxDataRecSize; n++)
    Buffer.R[n] = '\0';
  (*DatF).Header.FirstFree = -1;
  (*DatF).Header.ItemSize = RecLen;
  memcpy(TaRecBuf, (&(*DatF).Header), (size_t)FileHeaderSize);
  TaPutRec(DatF, DatE, R, TaRecBuf);
  (*DatF).Header.NumRec = 1;
}
/**************************************************************************/
void
    MakeFile(DatF, DatE, FName, RecLen, FileNum)

        DataFilePtr *DatF;
struct DataExt *DatE;
FileName FName;
unsigned long long RecLen;
long long FileNum;
{
  void WriteHeader();
  (*DatE).FileNumber = open(FName, O_CREAT | O_RDWR, 0666);
  if ((*DatE).FileNumber == -1)
  {
    IOStatus = ErrMakeFile;
    TAIOCheck();
  }
  else
  {
    if (RecLen > MaxDataRecSize)
    {
      IOStatus = RecTooLarge;
      TAIOCheck();
    }
    if (RecLen < MinDataRecSize)
    {
      IOStatus = RecTooSmall;
      TAIOCheck();
    }
    FindYourPlace(DatF, FileNum);
    (**DatF).Header.FirstFree = 0;
    (**DatF).Header.NumberFree = 0;
    (**DatF).Header.Int1 = 0;
    (**DatF).Header.ItemSize = 0;
    (**DatF).Header.NumRec = 0;
    (**DatF).FileNumber = (*DatE).FileNumber;

    strcpy((*DatE).FileTitle, FName);
    (*DatE).C = 1;

    WriteHeader(*DatF, DatE, RecLen);

    (*DatE).C = 0;
    (*DatE).H = 0;
  }
}
/**************************************************************************/
void
    MakeIndex(IdxF, IdxE, FName, KeyLen, S, FileNum)

        DataFilePtr *IdxF;
struct IndexExt *IdxE;
FileName FName;
unsigned long long KeyLen;
long long S;
long long FileNum;
{
  void WriteHeader();
  long long K, n;

  K = (KeyLen + ItemOverhead) * PageSize + PageOverhead;
  (*IdxE).DataE.FileNumber = open(FName, O_CREAT | O_RDWR, 0666);
  if ((*IdxE).DataE.FileNumber == -1)
  {
    IOStatus = ErrMakeIndex;
    TAIOCheck();
  }
  else
  {
    if (KeyLen > MaxKeyLen)
    {
      IOStatus = KeyTooLarge;
      TAIOCheck();
    }
    FindYourPlace(IdxF, FileNum);
    (**IdxF).Header.FirstFree = 0;
    (**IdxF).Header.NumberFree = 0;
    (**IdxF).Header.Int1 = 0;
    (**IdxF).Header.ItemSize = 0;
    (**IdxF).Header.NumRec = 0;
    (**IdxF).FileNumber = (*IdxE).DataE.FileNumber;
    (**IdxF).RR = 0;
    (**IdxF).KeyL = 0;

    strcpy((*IdxE).DataE.FileTitle, FName);
    (*IdxE).DataE.C = 1;
    (*IdxE).AllowDuplKeys = F;
    (*IdxE).PP = -1;
    for (n = 0; n < MaxHeight; n++)
    {
      (*IdxE).Path[n].PageRef = 0;
      (*IdxE).Path[n].ItemArrIndex = -1;
    }

    WriteHeader(*IdxF, &((*IdxE).DataE), K);

    (*IdxE).DataE.C = 0;
    (*IdxE).DataE.H = 0;
    if (S != NoDuplicates)
      (*IdxE).AllowDuplKeys = T;
    else
      (*IdxE).AllowDuplKeys = F;
    (**IdxF).KeyL = KeyLen;
  }
}
/**************************************************************************/
