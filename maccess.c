/**************************************************************************/
/* PROGRAM: MACCESS.C                                                     */
/* DESCRIPTION:  Basic modul; basic functions and search                  */
/* AUTHOR: VLADAN DJORDJEVIC                                              */
/**************************************************************************/
#include "defs.h"
#include <fcntl.h>
#include <unistd.h> 
#include <memory.h>
#include "mainter.h"

long long HadBits; /* It used to communicate between TaGetPage as one side 
		    * and NextKey and PrevKey on the other side! */
typedef char PageBlock[INT_MAX];
typedef PageBlock *pt; /* types */

TaPageStack *TaPageStk[HasMemories];
TaPageMap *TaPgMap[HasMemories];
long long IOStatus = -1;
union TaRecordBuffer Buffer;
union TaRecordBuffer *TaRecBuf;
Boolean OKAY = T;

/********************** LOW - level functions *****************************/
void
TAIOCheck ()
{
  char *Message[] = {"Record too large", /*  0 */
    "Record too small", /*  1 */
    "Key too large", /*  2 */
    "Record size mismatch", /*  3 */
    "Key size mismatch", /*  4 */
    "Make file error", /*  5 */
    "Header file read error", /*  6 */
    "Open file error", /*  7 */
    "Close file error", /*  8 */
    "Flush file error", /*  9 */
    "Make index error", /* 10 */
    "Open index error", /* 11 */
    "Delete record error", /* 12 */
    "Get record error", /* 13 */
    "Put record error", /* 14 */
    "Writing data error", /* 15 */
    "Wrong locking", /* 16 */
    "Lock error", /* 17 */
    "UnLock error", /* 18 */
    "Wrong unlocking", /* 19 */
    "Shared memory create error", /* 20 */
    "Shared memory attach error", /* 21 */
    "Shared memory detach error", /* 22 */
    "Shared memory erase error", /* 23 */
    "MemoryCode must have three digits"}; /* 24 */


  logMessage ("Maccess: F A T A L   E R R O R: %s", Message[IOStatus]);
}
/**************************************************************************/
void
GetRec (DatF, DatE, R, Buffer)

struct DataFile *DatF;
struct DataExt *DatE;
long long R;
void *Buffer;
{
  TaGetRec (DatF, DatE, R, Buffer);
}
/*************************************************************************/
void
PutRec (DatF, DatE, R, Buffer)

struct DataFile *DatF;
struct DataExt *DatE;
long long R;
void *Buffer;
{
  TaPutRec (DatF, DatE, R, Buffer);
}
/**************************************************************************/
void
TaGetRec (DatF, DatE, R, Buffer)

struct DataFile *DatF;
struct DataExt *DatE;
long long R;
void *Buffer;
{
  R = R * (*DatF).Heder.ItemSize;
  if (lseek ((*DatE).FileNumber, R, 0) == -1)
    {
      IOStatus = ErrGetRec;
      TAIOCheck ();
    }
  if (read ((*DatE).FileNumber, Buffer, (*DatF).Heder.ItemSize) == -1)
    {
      IOStatus = ErrGetRec;
      TAIOCheck ();
    }
}
/*************************************************************************/
void
TaPutRec (DatF, DatE, R, Buffer)

struct DataFile *DatF;
struct DataExt *DatE;
long long R;
void *Buffer;
{
  R = R * (*DatF).Heder.ItemSize;
  if (lseek ((*DatE).FileNumber, R, 0) == -1)
    {
      IOStatus = ErrPutRec;
      TAIOCheck ();
    }
  if (write ((*DatE).FileNumber, Buffer, (*DatF).Heder.ItemSize) == -1)
    {
      IOStatus = ErrPutRec;
      TAIOCheck ();
    }
}
/**************************************************************************/
void
ReadHeader (DatF, DatE)

struct DataFile *DatF;
struct DataExt *DatE;
{

  (*DatF).Heder.NumRec = lseek ((*DatE).FileNumber, (long long) 0, 2) /
          (*DatF).Heder.ItemSize;
  if ((*DatF).Heder.NumRec < 0)
    {
      IOStatus = ErrReadHeader;
      TAIOCheck ();
    }
  TaGetRec (DatF, DatE, (long long) 0, TaRecBuf);
  memcpy (&((*DatF).Heder), TaRecBuf, (size_t)FileHeaderSize);
}
/**************************************************************************/
void
OpenFile (DatF, DatE, FName, RecLen, MemNum, FileNum)

DataFilePtr *DatF;
struct DataExt *DatE;
FileName FName;
unsigned long long RecLen;
long long MemNum;
long long FileNum;
{
  void ReadHeader ();

  (*DatE).FileNumber = open (FName, O_RDWR);
  if ((*DatE).FileNumber == -1)
    {
      IOStatus = ErrOpenFile;
      TAIOCheck ();
    }
  else
    {
      (*DatE).C = 0;
      (*DatE).H = 0;
      strcpy ((*DatE).FileTitle, FName);
      if (RecLen > MaxDataRecSize)
        {
          IOStatus = RecTooLarge;
          TAIOCheck ();
        }
      if (RecLen < MinDataRecSize)
        {
          IOStatus = RecTooSmall;
          TAIOCheck ();
        }
      if (FindYourPlace (DatF, MemNum, FileNum) == 0)
        {
          (**DatF).M = MemNum;
          (**DatF).FileNumber = (*DatE).FileNumber;
          (**DatF).Heder.FirstFree = 0;
          (**DatF).Heder.NumberFree = 0;
          (**DatF).Heder.Int1 = 0;
          (**DatF).Heder.ItemSize = RecLen;
          (**DatF).Heder.NumRec = 0;
          ReadHeader (*DatF, DatE);
        }
    }
}
/**************************************************************************/
void
PutFileHeader (DatF, DatE)

struct DataFile *DatF;
struct DataExt *DatE;
{
  unsigned long long n;

  for (n = 0; n < MaxDataRecSize; n++)
    Buffer.R[n] = '\0';
  memcpy (TaRecBuf, &((*DatF).Heder), (size_t)FileHeaderSize);
  TaPutRec (DatF, DatE, (long long) 0, TaRecBuf);
}
/*************************************************************************/
void
CloseFile (DatF, DatE)

struct DataFile *DatF;
struct DataExt *DatE;
{
  void PutFileHeader ();

  PutFileHeader (DatF, DatE);
  if (close ((*DatE).FileNumber) == -1)
    {
      IOStatus = ErrCloseFile;
      TAIOCheck ();
    }
  else
    OKAY = T;
}
/**************************************************************************/
void
NewRec (DatF, R)

struct DataFile *DatF;
long long *R;
{

  if ((*DatF).Heder.FirstFree == -1)
    {
      *R = (*DatF).Heder.NumRec;
      (*DatF).Heder.NumRec++;
    }
  else
    {
      *R = (*DatF).Heder.FirstFree;
      (*DatF).Heder.FirstFree = (*TaRecBuf).I;
      (*DatF).Heder.NumberFree--;
    }
}
/**************************************************************************/
long long
FileLen (DatF)

struct DataFile *DatF;
{
  long long L;

  L = (*DatF).Heder.NumRec;
  return (L);
}
/**************************************************************************/
long long
UsedRecs (DatF)

struct DataFile *DatF;
{
  long long L;

  L = (*DatF).Heder.NumRec - (*DatF).Heder.NumberFree - 1;
  return (L);
}
/**************************************************************************/
void
TaPack (Page, KeyL) /***** rv *****/

void *Page;
unsigned long long KeyL;
{
  long long i, j, k;
  char *P;

  P = (char *) Page;
  j = (KeyL + ItemOverhead);

  if (KeyL != MaxKeyLen)
    for (i = 0; i < PageSize; i++)
      {
        k = (i * j + PageOverhead);
        memcpy ((pt) & P[k], &((*(struct TaPage *) Page).ItemArray[i]), j);
      }
}
/**************************************************************************/
void
TaUnPack (Page, KeyL)

void *Page;
unsigned long long KeyL;
{
  long long i, j, k;
  char *P;
  struct TaItem Item;

  P = (char *) Page;
  j = (KeyL + ItemOverhead);

  if (KeyL != MaxKeyLen)
    for (i = PageSize - 1; i >= 0; i--)
      {
        k = (i * j + PageOverhead);
        memcpy (&Item, (pt) & P[k], j);
        memcpy (&((*(struct TaPage *) Page).ItemArray[i]), &Item, j);
      }
}
/**************************************************************************/
void
OpenIndex (IdxF, IdxE, FName, KeyLen, S, MemNum, FileNum)

DataFilePtr *IdxF;
struct IndexExt *IdxE;
FileName FName;
unsigned long long KeyLen;
long long S;
long long MemNum;
long long FileNum;
{
  void ReadHeader ();
  unsigned long long K, n;

  K = (KeyLen + ItemOverhead) * PageSize + PageOverhead;
  (*IdxE).DataE.FileNumber = open (FName, O_RDWR);
  if ((*IdxE).DataE.FileNumber == -1)
    {
      IOStatus = ErrOpenIndex;
      TAIOCheck ();
    }
  else
    {
      (*IdxE).DataE.C = 0;
      (*IdxE).DataE.H = 0;
      strcpy ((*IdxE).DataE.FileTitle, FName);
      if (KeyLen > MaxKeyLen)
        {
          IOStatus = KeyTooLarge;
          TAIOCheck ();
        }
      if (FindYourPlace (IdxF, MemNum, FileNum) == 0)
        {
          (**IdxF).M = MemNum;
          (**IdxF).FileNumber = (*IdxE).DataE.FileNumber;
          (**IdxF).Heder.FirstFree = 0;
          (**IdxF).Heder.NumberFree = 0;
          (**IdxF).Heder.Int1 = 0;
          (**IdxF).Heder.ItemSize = K;
          (**IdxF).Heder.NumRec = 0;
          (**IdxF).RR = 0;
          (**IdxF).KeyL = KeyLen;
          ReadHeader (*IdxF, &((*IdxE).DataE));
          (**IdxF).RR = (**IdxF).Heder.Int1;
        }
      for (n = 0; n < MaxHeight; n++)
        {
          (*IdxE).Path[n].PageRef = 0;
          (*IdxE).Path[n].ItemArrIndex = -1;
        }
      if (S != NoDuplicates)
        (*IdxE).AllowDuplKeys = T;
      else
        (*IdxE).AllowDuplKeys = F;
      (*IdxE).PP = -1;
    }
}
/**************************************************************************/
void
StoreIndexHeader (IdxF, IdxE)

struct DataFile *IdxF;
struct IndexExt *IdxE;
{
  void TaPack ();
  long long i;

  for (i = 0; i < PageStackSize; i++)
    if ((*TaPageStk[(*IdxF).M])[i].IndexFPtr == IdxF)
      {
        (*TaPageStk[(*IdxF).M])[i].IndexFPtr = NULL;
        (*TaPageStk[(*IdxF).M])[i].IndexEPtr = NULL;
        if ((*TaPageStk[(*IdxF).M])[i].Updated == T)
          {
            TaPack (&((*TaPageStk[(*IdxF).M])[i].Page), (*IdxF).KeyL);
            TaPutRec (IdxF, &((*IdxE).DataE), (*TaPageStk[(*IdxF).M])[i].PageRef,
                      &((*TaPageStk[(*IdxF).M])[i].Page));
            (*TaPageStk[(*IdxF).M])[i].Updated = F;
          }
      }
  (*IdxF).Heder.Int1 = (*IdxF).RR;
}
/**************************************************************************/
void
CloseIndex (IdxF, IdxE)

struct DataFile *IdxF;
struct IndexExt *IdxE;
{
  void StoreIndexHeader ();
  void PutFileHeader ();

  StoreIndexHeader (IdxF, IdxE);
  PutFileHeader (IdxF, &((*IdxE).DataE));
  if (close ((*IdxE).DataE.FileNumber) == -1)
    {
      IOStatus = ErrCloseFile;
      TAIOCheck ();
    }
  else
    OKAY = T;
}
/**************************************************************************/
void
TaLast (i, MemNum)

long long i;
long long MemNum;
{
  long long j;

  for (j = 0; ((*TaPgMap[MemNum])[j] != i) && (j < PageStackSize - 1); j++)
    ;
  for (; j < PageStackSize - 1; j++)
    (*TaPgMap[MemNum])[j] = (*TaPgMap[MemNum])[j + 1];
  (*TaPgMap[MemNum])[PageStackSize - 1] = i;
}
/**************************************************************************/
void
TaReadPage (IdxF, IdxE, R, PagPtr)

struct DataFile *IdxF;
struct IndexExt *IdxE;
long long R;
TaPagePtr *PagPtr;
{
  void TaPack (), TaUnPack (), TaLast ();
  TaStackRecPtr TempPagPtr;
  long long i, num;
  Boolean Found;

  for (i = 0, Found = F; i < PageStackSize; i++)
    {
      if ((*TaPageStk[(*IdxF).M])[i].IndexFPtr == IdxF &&
          (*TaPageStk[(*IdxF).M])[i].PageRef == R)
        {
          Found = T;
          break;
        }
    }
  if (Found == F)
    {
      TempPagPtr = (TaStackRecPtr) malloc (sizeof (struct TaStackRec));
      TaGetRec (IdxF, &((*IdxE).DataE), R, &((*TempPagPtr).Page));
      TaUnPack (&((*TempPagPtr).Page), (*IdxF).KeyL);
      for (num = 0, HadBits = 0; num < (*TempPagPtr).Page.ItemsOnPage; num++)
        HadBits += (*TempPagPtr).Page.ItemArray[num].DataRef;
      if (HadBits == (*IdxE).Path[(long long) (*IdxE).PP].HasBits)
        {
          i = (*TaPgMap[(*IdxF).M])[0];
          if ((*TaPageStk[(*IdxF).M])[i].Updated == T)
            {
              TaPack (&((*TaPageStk[(*IdxF).M])[i].Page),
                      (*(*TaPageStk[(*IdxF).M])[i].IndexFPtr).KeyL);
              TaPutRec ((*TaPageStk[(*IdxF).M])[i].IndexFPtr,
                        &((*(*TaPageStk[(*IdxF).M])[i].IndexEPtr).DataE),
                        (*TaPageStk[(*IdxF).M])[(*IdxF).M].PageRef,
                        &((*TaPageStk[(*IdxF).M])[i].Page));
            }
          ((*TaPageStk[(*IdxF).M])[i]) = *TempPagPtr;
          (*TaPageStk[(*IdxF).M])[i].IndexFPtr = IdxF;
          (*TaPageStk[(*IdxF).M])[i].IndexEPtr = IdxE;
          (*TaPageStk[(*IdxF).M])[i].PageRef = R;
          (*TaPageStk[(*IdxF).M])[i].Updated = F;
        }
      else
        return;
    }
  else
    {
      TempPagPtr = &((*TaPageStk[(*IdxF).M])[i]);
      for (num = 0, HadBits = 0; num < (*TempPagPtr).Page.ItemsOnPage; num++)
        HadBits += (*TempPagPtr).Page.ItemArray[num].DataRef;
    }
  TaLast (i, (*IdxF).M);
  *PagPtr = (TaPagePtr) &((*TaPageStk[(*IdxF).M])[i]);
}
/*************************************************************************/
void
TaWriteRec (DatF, R, Buffer)

struct DataFile *DatF;
long long R;
void *Buffer;
{
  R = R * (*DatF).Heder.ItemSize;
  if (lseek ((*DatF).FileNumber, R, 0) == -1)
    {
      IOStatus = ErrWriteRec;
      TAIOCheck ();
    }
  if (write ((*DatF).FileNumber, Buffer, (*DatF).Heder.ItemSize) == -1)
    {
      IOStatus = ErrWriteRec;
      TAIOCheck ();
    }
}
/**************************************************************************/
void
TaGetPage (IdxF, IdxE, R, PagPtr)

struct DataFile *IdxF;
struct IndexExt *IdxE;
long long R;
TaPagePtr *PagPtr;
{
  void TaPack (), TaUnPack (), TaLast (), TaWriteRec ();
  long long i;
  Boolean Found;

  for (i = 0, Found = F; i < PageStackSize; i++)
    {
      if ((*TaPageStk[(*IdxF).M])[i].IndexFPtr == IdxF &&
          (*TaPageStk[(*IdxF).M])[i].PageRef == R)
        {
          Found = T;
          break;
        }
    }
  if (Found == F)
    {
      i = (*TaPgMap[(*IdxF).M])[0];
      if ((*TaPageStk[(*IdxF).M])[i].Updated == T)
        {
          TaPack (&((*TaPageStk[(*IdxF).M])[i].Page),
                  (*(*TaPageStk[(*IdxF).M])[i].IndexFPtr).KeyL);
          TaWriteRec ((*TaPageStk[(*IdxF).M])[i].IndexFPtr,
                      (*TaPageStk[(*IdxF).M])[i].PageRef,
                      &((*TaPageStk[(*IdxF).M])[i].Page));
        }
      TaGetRec (IdxF, &((*IdxE).DataE), R, &((*TaPageStk[(*IdxF).M])[i].Page));
      TaUnPack (&((*TaPageStk[(*IdxF).M])[i].Page), (*IdxF).KeyL);
      (*TaPageStk[(*IdxF).M])[i].IndexFPtr = IdxF;
      (*TaPageStk[(*IdxF).M])[i].IndexEPtr = IdxE;
      (*TaPageStk[(*IdxF).M])[i].PageRef = R;
      (*TaPageStk[(*IdxF).M])[i].Updated = F;
    }
  TaLast (i, (*IdxF).M);
  *PagPtr = (TaPagePtr) &((*TaPageStk[(*IdxF).M])[i]);
}
/**************************************************************************/
void
TaNewPage (IdxF, IdxE, R, PagPtr)

struct DataFile *IdxF;
struct IndexExt *IdxE;
long long *R;
TaPagePtr *PagPtr;
{
  void TaPack (), TaLast (), NewRec (), TaWriteRec ();
  long long i;

  i = (*TaPgMap[(*IdxF).M])[0];
  if ((*TaPageStk[(*IdxF).M])[i].Updated == T)
    {
      TaPack (&((*TaPageStk[(*IdxF).M])[i].Page),
              (*(*TaPageStk[(*IdxF).M])[i].IndexFPtr).KeyL);
      TaWriteRec ((*TaPageStk[(*IdxF).M])[i].IndexFPtr,
                  (*TaPageStk[(*IdxF).M])[i].PageRef,
                  &((*TaPageStk[(*IdxF).M])[i].Page));
    }
  NewRec (IdxF, R);
  (*TaPageStk[(*IdxF).M])[i].IndexFPtr = IdxF;
  (*TaPageStk[(*IdxF).M])[i].IndexEPtr = IdxE;
  (*TaPageStk[(*IdxF).M])[i].PageRef = *R;
  (*TaPageStk[(*IdxF).M])[i].Updated = F;
  TaLast (i, (*IdxF).M);
  *PagPtr = (TaPagePtr) &((*TaPageStk[(*IdxF).M])[i]);
}
/**************************************************************************/
void
TaUpdatePage (PagePtr)

TaPagePtr PagePtr;
{
  (*((TaStackRecPtr) PagePtr)).Updated = T;
}
/**************************************************************************/
long long
TaCompKeys (K1, K2, DR1, DR2, DUP)

TaKeyStr *K1;
TaKeyStr K2;
long long DR1, DR2;
Boolean DUP;
{
  if (strcmp (*K1, K2) == 0)
    if (DUP == T)
      return (DR1 - DR2);
    else
      return (0);
  else
    if (strcmp (*K1, K2) > 0)
    return (1);
  else
    return (-1);
}
/**************************************************************************/
void
CutString (ArrayOfSymbols, Length)

char *ArrayOfSymbols;
long unsigned int Length;
{
  if (strlen (ArrayOfSymbols) > Length)
    *(ArrayOfSymbols + Length) = '\0';
}
/**************************************************************************/
void
ClearKey (IdxE)

struct IndexExt *IdxE;
{
  (*IdxE).PP = -1;
}
/**************************************************************************/
void
NextKey (IdxF, IdxE, DataRecNum, ProcKey)

struct DataFile *IdxF;
struct IndexExt *IdxE;
long long *DataRecNum;
void *ProcKey;
{
  void TaReadPage ();
  TaKeyStr temp, provkljuc;
  long long num;
  long long R, provslog;
  TaPagePtr PagPtr = NULL;

  if ((*IdxE).PP == -1)
    R = (*IdxF).RR;
  else
    {
      TaReadPage (IdxF, IdxE, (*IdxE).Path[(long long) (*IdxE).PP].PageRef, &PagPtr);

      if (HadBits != (*IdxE).Path[(long long) (*IdxE).PP].HasBits)
        {
          strcpy (temp, (*IdxE).LastKey);
          strcpy (provkljuc, (*IdxE).LastKey);
          provslog = (*IdxE).LastRecord;
          FindKey (IdxF, IdxE, DataRecNum, temp);
          if (OKAY == T)
            {
              while (provslog != (*IdxE).LastRecord && strcmp (temp, provkljuc) == 0)
                NextKey (IdxF, IdxE, DataRecNum, temp);
              NextKey (IdxF, IdxE, DataRecNum, temp);
            }
          else
            SearchKey (IdxF, IdxE, DataRecNum, temp);
          strcpy (*(TaKeyStr *) ProcKey, temp);
          return;
        }
      R = (*PagPtr).ItemArray
              [(long long) (*IdxE).Path[(long long) (*IdxE).PP].ItemArrIndex].PageRef;
    }
  while (R != 0)
    {
      (*IdxE).PP++;
      (*IdxE).Path[(long long) (*IdxE).PP].PageRef = R;
      (*IdxE).Path[(long long) (*IdxE).PP].ItemArrIndex = -1;
      TaGetPage (IdxF, IdxE, R, &PagPtr);
      R = (*PagPtr).BckwPageRef;

      (*IdxE).Path[(long long) (*IdxE).PP].HasBits = 0;
      for (num = 0; num < (*PagPtr).ItemsOnPage; num++)
        (*IdxE).Path[(long long) (*IdxE).PP].HasBits +=
              (*PagPtr).ItemArray[num].DataRef;

    }
  if ((*IdxE).PP != -1)
    {
      while ((*IdxE).PP > 0 &&
             (*IdxE).Path[(long long) (*IdxE).PP].ItemArrIndex ==
             (*PagPtr).ItemsOnPage - 1)
        {
          (*IdxE).PP--;
          TaGetPage (IdxF, IdxE, (*IdxE).Path[(long long) (*IdxE).PP].PageRef, &PagPtr);

          for (num = 0, HadBits = 0; num < (*PagPtr).ItemsOnPage; num++)
            HadBits += (*PagPtr).ItemArray[num].DataRef;
          if (HadBits != (*IdxE).Path[(long long) (*IdxE).PP].HasBits)
            {
              strcpy (temp, (*IdxE).LastKey);
              strcpy (provkljuc, (*IdxE).LastKey);
              provslog = (*IdxE).LastRecord;
              FindKey (IdxF, IdxE, DataRecNum, temp);
              if (OKAY == T)
                {
                  while (provslog != (*IdxE).LastRecord &&
                         strcmp (temp, provkljuc) == 0)
                    NextKey (IdxF, IdxE, DataRecNum, temp);
                  NextKey (IdxF, IdxE, DataRecNum, temp);
                }
              else
                SearchKey (IdxF, IdxE, DataRecNum, temp);
              strcpy (*(TaKeyStr *) ProcKey, temp);
              return;
            }
        }
      if ((*IdxE).Path[(long long) (*IdxE).PP].ItemArrIndex <
          (*PagPtr).ItemsOnPage - 1)
        {
          (*IdxE).Path[(long long) (*IdxE).PP].ItemArrIndex++;
          strcpy (*(TaKeyStr *) ProcKey, (*PagPtr).ItemArray
                  [(long long) (*IdxE).Path[(long long) (*IdxE).PP].ItemArrIndex].Key);
          *DataRecNum = (*PagPtr).ItemArray
                  [(long long) (*IdxE).Path[(long long) (*IdxE).PP].ItemArrIndex].DataRef;

          (*IdxE).LastRecord = *DataRecNum;
          strcpy ((*IdxE).LastKey, *(TaKeyStr *) ProcKey);
        }
      else
        (*IdxE).PP = -1;
    }
  if ((*IdxE).PP != -1)
    OKAY = T;
  else
    OKAY = F;
}
/**************************************************************************/
void
PrevKey (IdxF, IdxE, DataRecNum, ProcKey)

struct DataFile *IdxF;
struct IndexExt *IdxE;
long long *DataRecNum;
void *ProcKey;
{
  void TaReadPage ();
  TaKeyStr temp, provkljuc;
  long long num;
  long long R, provslog;
  TaPagePtr PagPtr = NULL;

  if ((*IdxE).PP == -1)
    R = (*IdxF).RR;
  else
    {
      TaReadPage (IdxF, IdxE, (*IdxE).Path[(long long) (*IdxE).PP].PageRef, &PagPtr);

      if (HadBits != (*IdxE).Path[(long long) (*IdxE).PP].HasBits)
        {
          strcpy (temp, (*IdxE).LastKey);
          strcpy (provkljuc, (*IdxE).LastKey);
          provslog = (*IdxE).LastRecord;
          FindKey (IdxF, IdxE, DataRecNum, temp);
          if (OKAY == T)
            {
              while (provslog != (*IdxE).LastRecord && strcmp (temp, provkljuc) == 0)
                NextKey (IdxF, IdxE, DataRecNum, temp);
              PrevKey (IdxF, IdxE, DataRecNum, temp);
            }
          else
            {
              SearchKey (IdxF, IdxE, DataRecNum, temp);
              PrevKey (IdxF, IdxE, DataRecNum, temp);
            }
          strcpy (*(TaKeyStr *) ProcKey, temp);
          return;
        }
      (*IdxE).Path[(long long) (*IdxE).PP].ItemArrIndex--;
      if ((*IdxE).Path[(long long) (*IdxE).PP].ItemArrIndex == -1)
        R = (*PagPtr).BckwPageRef;
      else
        R = (*PagPtr).ItemArray
              [(long long) (*IdxE).Path[(long long) (*IdxE).PP].ItemArrIndex].PageRef;
    }
  while (R != 0)
    {
      TaGetPage (IdxF, IdxE, R, &PagPtr);
      (*IdxE).PP++;
      (*IdxE).Path[(long long) (*IdxE).PP].PageRef = R;
      (*IdxE).Path[(long long) (*IdxE).PP].ItemArrIndex = (*PagPtr).ItemsOnPage - 1;
      R = (*PagPtr).ItemArray[(*PagPtr).ItemsOnPage - 1].PageRef;

      (*IdxE).Path[(long long) (*IdxE).PP].HasBits = 0;
      for (num = 0; num < (*PagPtr).ItemsOnPage; num++)
        (*IdxE).Path[(long long) (*IdxE).PP].HasBits +=
              (*PagPtr).ItemArray[num].DataRef;
    }
  if ((*IdxE).PP != -1)
    {
      while ((*IdxE).PP > 0 &&
             (*IdxE).Path[(long long) (*IdxE).PP].ItemArrIndex == -1)
        {
          (*IdxE).PP--;
          TaGetPage (IdxF, IdxE, (*IdxE).Path[(long long) (*IdxE).PP].PageRef, &PagPtr);

          for (num = 0, HadBits = 0; num < (*PagPtr).ItemsOnPage; num++)
            HadBits += (*PagPtr).ItemArray[num].DataRef;
          if (HadBits != (*IdxE).Path[(long long) (*IdxE).PP].HasBits)
            {
              strcpy (temp, (*IdxE).LastKey);
              strcpy (provkljuc, (*IdxE).LastKey);
              provslog = (*IdxE).LastRecord;
              FindKey (IdxF, IdxE, DataRecNum, temp);
              if (OKAY == T)
                {
                  while (provslog != (*IdxE).LastRecord &&
                         strcmp (temp, provkljuc) == 0)
                    NextKey (IdxF, IdxE, DataRecNum, temp);
                  PrevKey (IdxF, IdxE, DataRecNum, temp);
                }
              else
                {
                  SearchKey (IdxF, IdxE, DataRecNum, temp);
                  PrevKey (IdxF, IdxE, DataRecNum, temp);
                }
              strcpy (*(TaKeyStr *) ProcKey, temp);
              return;
            }
        }
      if ((*IdxE).Path[(long long) (*IdxE).PP].ItemArrIndex > -1)
        {
          strcpy (*(TaKeyStr *) ProcKey, (*PagPtr).ItemArray
                  [(long long) (*IdxE).Path[(long long) (*IdxE).PP].ItemArrIndex].Key);
          *DataRecNum = (*PagPtr).ItemArray
                  [(long long) (*IdxE).Path[(long long) (*IdxE).PP].ItemArrIndex].DataRef;

          (*IdxE).LastRecord = *DataRecNum;
          strcpy ((*IdxE).LastKey, *(TaKeyStr *) ProcKey);
        }
      else
        (*IdxE).PP = -1;
    }
  if ((*IdxE).PP != -1)
    OKAY = T;
  else
    OKAY = F;
}
/**************************************************************************/
void
TaFindKey (IdxF, IdxE, DataRecNum, ProcKey)

struct DataFile *IdxF;
struct IndexExt *IdxE;
long long *DataRecNum;
void *ProcKey;
{
  void TaGetPage (), CutString ();
  long long TaCompKeys ();

  long long PrPgRef, c;
  long long num, k, l, r;
  TaPagePtr PagPtr = NULL;

  if (strlen (*(TaKeyStr *) ProcKey) > (*IdxF).KeyL)
    CutString (*(TaKeyStr *) ProcKey, (*IdxF).KeyL);
  OKAY = F;
  (*IdxE).PP = -1;
  PrPgRef = (*IdxF).RR;
  while (PrPgRef != 0 && OKAY != T)
    {
      (*IdxE).PP++;
      (*IdxE).Path[(long long) (*IdxE).PP].PageRef = PrPgRef;
      TaGetPage (IdxF, IdxE, PrPgRef, &PagPtr);

      (*IdxE).Path[(long long) (*IdxE).PP].HasBits = 0;
      for (num = 0; num < (*PagPtr).ItemsOnPage; num++)
        (*IdxE).Path[(long long) (*IdxE).PP].HasBits +=
              (*PagPtr).ItemArray[num].DataRef;

      l = 0;
      r = (*PagPtr).ItemsOnPage - 1;
      do
        {
          k = (long long) ((l + r) / 2);
          c = TaCompKeys ((TaKeyStr *) ProcKey,
                          (*PagPtr).ItemArray[k].Key,
                          (long long) 0,
                          (*PagPtr).ItemArray[k].DataRef,
                          (*IdxE).AllowDuplKeys);
          if (c <= 0)
            r = k - 1;
          if (c >= 0)
            l = k + 1;
        }
      while (r >= l);
      if (l - r > 1)
        {
          *DataRecNum = (*PagPtr).ItemArray[k].DataRef;

          (*IdxE).LastRecord = *DataRecNum;
          strcpy ((*IdxE).LastKey, (*PagPtr).ItemArray[k].Key);

          r = k;
          OKAY = T;
        }
      if (r == -1)
        PrPgRef = (*PagPtr).BckwPageRef;
      else
        PrPgRef = (*PagPtr).ItemArray[r].PageRef;
      (*IdxE).Path[(long long) (*IdxE).PP].ItemArrIndex = r;
    }
  if (OKAY != T && (*IdxE).PP > -1)
    {
      while
        ((*IdxE).PP > 0 && (*IdxE).Path[(long long) (*IdxE).PP].ItemArrIndex == -1)
        (*IdxE).PP--;
      if ((*IdxE).Path[(long long) (*IdxE).PP].ItemArrIndex == -1)
        (*IdxE).PP = -1;
    }
}
/**************************************************************************/
void
FindKey (IdxF, IdxE, DataRecNum, ProcKey)

struct DataFile *IdxF;
struct IndexExt *IdxE;
long long *DataRecNum;
void *ProcKey;
{
  void TaFindKey ();
  TaKeyStr TempKey;

  *DataRecNum = 0;

  TaFindKey (IdxF, IdxE, DataRecNum, ProcKey);
  if (OKAY != T && (*IdxE).AllowDuplKeys == T)
    {
      strcpy (TempKey, *(TaKeyStr *) ProcKey);
      NextKey (IdxF, IdxE, DataRecNum, ProcKey);
      if (!(OKAY == T && strcmp (*(TaKeyStr *) ProcKey, TempKey) == 0))
        OKAY = F;
    }
}
/**************************************************************************/
void
SearchKey (IdxF, IdxE, DataRecNum, ProcKey)

struct DataFile *IdxF;
struct IndexExt *IdxE;
long long *DataRecNum;
void *ProcKey;
{
  void TaFindKey ();

  *DataRecNum = 0;
  TaFindKey (IdxF, IdxE, DataRecNum, ProcKey);
  if (OKAY != T)
    NextKey (IdxF, IdxE, DataRecNum, ProcKey);
}
/**************************************************************************/
