/**************************************************************************/
/* PROGRAM: MACCESS.C                                                     */
/* DESCRIPTION:  Basic modul; basic functions and search                  */
/* AUTHOR: VLADAN DJORDJEVIC                                              */
/**************************************************************************/
#include "maccess.h"
#include <fcntl.h>
# if defined(__linux__) || defined (unix) || defined (__unix) || defined(__unix__)
#include <memory.h>
#endif

long long HadBits; /* It used to communicate between TaGetPage as one side 
		    * and NextKey and PrevKey on the other side! */
typedef char PageBlock[INT_MAX];
/* typedef char PageBlock[LLONG_MAX]; */ /* Can do, but no need */
typedef PageBlock *pt; /* types */

TaPageStack *TaPageStk;
TaPageMap *TaPgMap;
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
unsigned long long R;
void *Buffer;
{
  TaGetRec (DatF, DatE, R, Buffer);
}
/*************************************************************************/
void
PutRec (DatF, DatE, R, Buffer)

struct DataFile *DatF;
struct DataExt *DatE;
unsigned long long R;
void *Buffer;
{
  TaPutRec (DatF, DatE, R, Buffer);
}
/**************************************************************************/
void
TaGetRec (DatF, DatE, R, Buffer)

struct DataFile *DatF;
struct DataExt *DatE;
unsigned long long R;
void *Buffer;
{
  R = R * (*DatF).Header.ItemSize;
  if (lseek64 ((*DatE).FileNumber, R, 0) == -1)
    {
      IOStatus = ErrGetRec;
      TAIOCheck ();
    }
  if (read ((*DatE).FileNumber, Buffer, (*DatF).Header.ItemSize) == -1)
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
unsigned long long R;
void *Buffer;
{
  R = R * (*DatF).Header.ItemSize;
  if (lseek64 ((*DatE).FileNumber, R, 0) == -1)
    {
      IOStatus = ErrPutRec;
      TAIOCheck ();
    }
  if (write ((*DatE).FileNumber, Buffer, (*DatF).Header.ItemSize) == -1)
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
  (*DatF).Header.NumRec = lseek64 ((*DatE).FileNumber,  0, 2) / \
			  (*DatF).Header.ItemSize;
  if ((*DatF).Header.NumRec < 0)
    {
      IOStatus = ErrReadHeader;
      TAIOCheck ();
    }
  TaGetRec (DatF, DatE, 0, TaRecBuf);
  memcpy (&((*DatF).Header), TaRecBuf, (size_t)FileHeaderSize);
}
/**************************************************************************/
void
OpenFile (DatF, DatE, FName, RecLen, FileNum)

DataFilePtr *DatF;
struct DataExt *DatE;
FileName FName;
unsigned long long RecLen;
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
      if (FindYourPlace (DatF, FileNum) == 0)
        {
          (**DatF).FileNumber = (*DatE).FileNumber;
          (**DatF).Header.FirstFree = 0;
          (**DatF).Header.NumberFree = 0;
          (**DatF).Header.Int1 = 0;
          (**DatF).Header.ItemSize = RecLen;
          (**DatF).Header.NumRec = 0;
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
  memcpy (TaRecBuf, &((*DatF).Header), (size_t)FileHeaderSize);
  TaPutRec (DatF, DatE, 0, TaRecBuf);
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
size_t *R;
{

  if ((*DatF).Header.FirstFree == -1)
    {
      *R = (*DatF).Header.NumRec;
      (*DatF).Header.NumRec++;
    }
  else
    {
      *R = (*DatF).Header.FirstFree;
      (*DatF).Header.FirstFree = (*TaRecBuf).I;
      (*DatF).Header.NumberFree--;
    }
}
/**************************************************************************/
long long
FileLen (DatF)

struct DataFile *DatF;
{
  long long L;

  L = (*DatF).Header.NumRec;
  return (L);
}
/**************************************************************************/
long long
UsedRecs (DatF)

struct DataFile *DatF;
{
  long long L;

  L = (*DatF).Header.NumRec - (*DatF).Header.NumberFree - 1;
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
        memcpy ((pt) & P[k], &((*(struct TaPage *) Page).ItemArray[i]), j); /* ovde */
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
OpenIndex (IdxF, IdxE, FName, KeyLen, S, FileNum)

DataFilePtr *IdxF;
struct IndexExt *IdxE;
FileName FName;
unsigned long long KeyLen;
long long S;
long long FileNum;
{
  void ReadHeader ();
  unsigned long long K;
  unsigned long long n;

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
      if (FindYourPlace (IdxF, FileNum) == 0)
        {
          (**IdxF).FileNumber = (*IdxE).DataE.FileNumber;
          (**IdxF).Header.FirstFree = 0;
          (**IdxF).Header.NumberFree = 0;
          (**IdxF).Header.Int1 = 0;
          (**IdxF).Header.ItemSize = K;
          (**IdxF).Header.NumRec = 0;
          (**IdxF).RR = 0;
          (**IdxF).KeyL = KeyLen;
          ReadHeader (*IdxF, &((*IdxE).DataE));
          (**IdxF).RR = (**IdxF).Header.Int1;
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
    if ((*TaPageStk)[i].IndexFPtr == IdxF)
      {
        (*TaPageStk)[i].IndexFPtr = NULL;
        (*TaPageStk)[i].IndexEPtr = NULL;
        if ((*TaPageStk)[i].Updated == T)
          {
            TaPack (&((*TaPageStk)[i].Page), (*IdxF).KeyL);
            TaPutRec (IdxF, &((*IdxE).DataE), (*TaPageStk)[i].PageRef,
                      &((*TaPageStk)[i].Page));
            (*TaPageStk)[i].Updated = F;
          }
      }
  (*IdxF).Header.Int1 = (*IdxF).RR;
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
TaLast (i)

long long i;
{
  long long j;

  for (j = 0; ((*TaPgMap)[j] != i) && (j < PageStackSize - 1); j++)
    ;
  for (; j < PageStackSize - 1; j++)
    (*TaPgMap)[j] = (*TaPgMap)[j + 1];
  (*TaPgMap)[PageStackSize - 1] = i;
}
/**************************************************************************/
void
TaReadPage (IdxF, IdxE, R, PagPtr)

struct DataFile *IdxF;
struct IndexExt *IdxE;
unsigned long long R;
TaPagePtr *PagPtr;
{
  void TaPack (), TaUnPack (), TaLast ();
  TaStackRecPtr TempPagPtr;
  long long i, num;

  for (i = 0; i < PageStackSize; i++)
    {
      if ((*TaPageStk)[i].IndexFPtr == IdxF &&
          (*TaPageStk)[i].PageRef == R)
          break;
    }
  TempPagPtr = &((*TaPageStk)[i]);
  for (num = 0, HadBits = 0; num < (*TempPagPtr).Page.ItemsOnPage; num++)
      HadBits += (*TempPagPtr).Page.ItemArray[num].DataRef;
  TaLast (i);
  *PagPtr = (TaPagePtr) &((*TaPageStk)[i]);
}
/*************************************************************************/
void
TaWriteRec (DatF, R, Buffer)

struct DataFile *DatF;
size_t R;
void *Buffer;
{
  R = R * (*DatF).Header.ItemSize;
  if (lseek64 ((*DatF).FileNumber, R, 0) == -1)
    {
      IOStatus = ErrWriteRec;
      TAIOCheck ();
    }
  if (write ((*DatF).FileNumber, Buffer, (*DatF).Header.ItemSize) == -1)
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
size_t R;
TaPagePtr *PagPtr;
{
  void TaPack (), TaUnPack (), TaLast (), TaWriteRec ();
  long long i;
  Boolean Found;

  for (i = 0, Found = F; i < PageStackSize; i++)
    {
      if ((*TaPageStk)[i].IndexFPtr == IdxF &&
          (*TaPageStk)[i].PageRef == R)
        {
          Found = T;
          break;
        }
    }
  if (Found == F)
    {
      i = (*TaPgMap)[0];
      if ((*TaPageStk)[i].Updated == T)
        {
          TaPack (&((*TaPageStk)[i].Page),
                  (*(*TaPageStk)[i].IndexFPtr).KeyL);
          TaWriteRec ((*TaPageStk)[i].IndexFPtr,
                      (*TaPageStk)[i].PageRef,
                      &((*TaPageStk)[i].Page));
        }
      TaGetRec (IdxF, &((*IdxE).DataE), R, &((*TaPageStk)[i].Page));
      TaUnPack (&((*TaPageStk)[i].Page), (*IdxF).KeyL);
      (*TaPageStk)[i].IndexFPtr = IdxF;
      (*TaPageStk)[i].IndexEPtr = IdxE;
      (*TaPageStk)[i].PageRef = R;
      (*TaPageStk)[i].Updated = F;
    }
  TaLast (i);
  *PagPtr = (TaPagePtr) &((*TaPageStk)[i]);
}
/**************************************************************************/
void
TaNewPage (IdxF, IdxE, R, PagPtr)

struct DataFile *IdxF;
struct IndexExt *IdxE;
size_t *R;
TaPagePtr *PagPtr;
{
  void TaPack (), TaLast (), NewRec (), TaWriteRec ();
  long long i;

  i = (*TaPgMap)[0];
  if ((*TaPageStk)[i].Updated == T)
    {
      TaPack (&((*TaPageStk)[i].Page),
              (*(*TaPageStk)[i].IndexFPtr).KeyL);
      TaWriteRec ((*TaPageStk)[i].IndexFPtr,
                  (*TaPageStk)[i].PageRef,
                  &((*TaPageStk)[i].Page));
    }
  NewRec (IdxF, R);
  (*TaPageStk)[i].IndexFPtr = IdxF;
  (*TaPageStk)[i].IndexEPtr = IdxE;
  (*TaPageStk)[i].PageRef = *R;
  (*TaPageStk)[i].Updated = F;
  TaLast (i);
  *PagPtr = (TaPagePtr) &((*TaPageStk)[i]);
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
unsigned long long DR1, DR2;
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
unsigned long long Length;
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
unsigned long long *DataRecNum;
void *ProcKey;
{
  void TaReadPage ();
  TaKeyStr temp, check_key;
  long long num;
  unsigned long long R, check_record;
  TaPagePtr PagPtr = NULL;

  if ((*IdxE).PP == -1)
    R = (*IdxF).RR;
  else
    {
      TaReadPage (IdxF, IdxE, (*IdxE).Path[(*IdxE).PP].PageRef, &PagPtr);

      if (HadBits != (*IdxE).Path[(*IdxE).PP].HasBits)
        {
          strcpy (temp, (*IdxE).LastKey);
          strcpy (check_key, (*IdxE).LastKey);
          check_record = (*IdxE).LastRecord;
          FindKey (IdxF, IdxE, DataRecNum, temp);
          if (OKAY == T)
            {
              while (check_record != (*IdxE).LastRecord && strcmp (temp, check_key) == 0)
                NextKey (IdxF, IdxE, DataRecNum, temp);
              NextKey (IdxF, IdxE, DataRecNum, temp);
            }
          else
            SearchKey (IdxF, IdxE, DataRecNum, temp);
          strcpy (*(TaKeyStr *) ProcKey, temp);
          return;
        }
      R = (*PagPtr).ItemArray
              [(*IdxE).Path[(*IdxE).PP].ItemArrIndex].PageRef;
    }
  while (R != 0)
    {
      (*IdxE).PP++;
      (*IdxE).Path[(*IdxE).PP].PageRef = R;
      (*IdxE).Path[(*IdxE).PP].ItemArrIndex = -1;
      TaGetPage (IdxF, IdxE, R, &PagPtr);
      R = (*PagPtr).BckwPageRef;

      (*IdxE).Path[(*IdxE).PP].HasBits = 0;
      for (num = 0; num < (*PagPtr).ItemsOnPage; num++)
        (*IdxE).Path[(*IdxE).PP].HasBits +=
              (*PagPtr).ItemArray[num].DataRef;

    }
  if ((*IdxE).PP != -1)
    {
      while ((*IdxE).PP > 0 &&
             (*IdxE).Path[(*IdxE).PP].ItemArrIndex ==
             (*PagPtr).ItemsOnPage - 1)
        {
          (*IdxE).PP--;
          TaGetPage (IdxF, IdxE, (*IdxE).Path[(*IdxE).PP].PageRef, &PagPtr);

          for (num = 0, HadBits = 0; num < (*PagPtr).ItemsOnPage; num++)
            HadBits += (*PagPtr).ItemArray[num].DataRef;
          if (HadBits != (*IdxE).Path[(*IdxE).PP].HasBits)
            {
              strcpy (temp, (*IdxE).LastKey);
              strcpy (check_key, (*IdxE).LastKey);
              check_record = (*IdxE).LastRecord;
              FindKey (IdxF, IdxE, DataRecNum, temp);
              if (OKAY == T)
                {
                  while (check_record != (*IdxE).LastRecord &&
                         strcmp (temp, check_key) == 0)
                    NextKey (IdxF, IdxE, DataRecNum, temp);
                  NextKey (IdxF, IdxE, DataRecNum, temp);
                }
              else
                SearchKey (IdxF, IdxE, DataRecNum, temp);
              strcpy (*(TaKeyStr *) ProcKey, temp);
              return;
            }
        }
      if ((*IdxE).Path[(*IdxE).PP].ItemArrIndex <
          (*PagPtr).ItemsOnPage - 1)
        {
          (*IdxE).Path[(*IdxE).PP].ItemArrIndex++;
          strcpy (*(TaKeyStr *) ProcKey, (*PagPtr).ItemArray
                  [(*IdxE).Path[(*IdxE).PP].ItemArrIndex].Key);
          *DataRecNum = (*PagPtr).ItemArray \
			[(*IdxE).Path[(*IdxE).PP].ItemArrIndex].DataRef;

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
unsigned long long *DataRecNum;
void *ProcKey;
{
  void TaReadPage ();
  TaKeyStr temp, check_key;
  unsigned long long num;
  unsigned long long R, check_record;

  TaPagePtr PagPtr = NULL;

  if ((*IdxE).PP == -1)
    R = (*IdxF).RR;
  else
    {
      TaReadPage (IdxF, IdxE, (*IdxE).Path[(*IdxE).PP].PageRef, &PagPtr);

      if (HadBits != (*IdxE).Path[(*IdxE).PP].HasBits)
        {
          strcpy (temp, (*IdxE).LastKey);
          strcpy (check_key, (*IdxE).LastKey);
          check_record = (*IdxE).LastRecord;
          FindKey (IdxF, IdxE, DataRecNum, temp);
          if (OKAY == T)
            {
              while (check_record != (*IdxE).LastRecord && strcmp (temp, check_key) == 0)
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
      (*IdxE).Path[(*IdxE).PP].ItemArrIndex--;
      if ((*IdxE).Path[(*IdxE).PP].ItemArrIndex == -1)
        R = (*PagPtr).BckwPageRef;
      else
        R = (*PagPtr).ItemArray
              [(*IdxE).Path[(*IdxE).PP].ItemArrIndex].PageRef;
    }
  while (R != 0)
    {
      TaGetPage (IdxF, IdxE, R, &PagPtr);
      (*IdxE).PP++;
      (*IdxE).Path[(*IdxE).PP].PageRef = R;
      (*IdxE).Path[(*IdxE).PP].ItemArrIndex = (*PagPtr).ItemsOnPage - 1;
      R = (*PagPtr).ItemArray[(*PagPtr).ItemsOnPage - 1].PageRef;

      (*IdxE).Path[(*IdxE).PP].HasBits = 0;
      for (num = 0; num < (*PagPtr).ItemsOnPage; num++)
        (*IdxE).Path[(*IdxE).PP].HasBits +=
              (*PagPtr).ItemArray[num].DataRef;
    }
  if ((*IdxE).PP != -1)
    {
      while ((*IdxE).PP > 0 &&
             (*IdxE).Path[(*IdxE).PP].ItemArrIndex == -1)
        {
          (*IdxE).PP--;
          TaGetPage (IdxF, IdxE, (*IdxE).Path[(*IdxE).PP].PageRef, &PagPtr);

          for (num = 0, HadBits = 0; num < (*PagPtr).ItemsOnPage; num++)
            HadBits += (*PagPtr).ItemArray[num].DataRef;
          if (HadBits != (*IdxE).Path[(*IdxE).PP].HasBits)
            {
              strcpy (temp, (*IdxE).LastKey);
              strcpy (check_key, (*IdxE).LastKey);
              check_record = (*IdxE).LastRecord;
              FindKey (IdxF, IdxE, DataRecNum, temp);
              if (OKAY == T)
                {
                  while (check_record != (*IdxE).LastRecord &&
                         strcmp (temp, check_key) == 0)
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
      if ((*IdxE).Path[(*IdxE).PP].ItemArrIndex > -1)
        {
          strcpy (*(TaKeyStr *) ProcKey, (*PagPtr).ItemArray
                  [(*IdxE).Path[(*IdxE).PP].ItemArrIndex].Key);
          *DataRecNum = (*PagPtr).ItemArray
                  [(*IdxE).Path[(*IdxE).PP].ItemArrIndex].DataRef;

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
unsigned long long *DataRecNum;
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
      (*IdxE).Path[(*IdxE).PP].PageRef = PrPgRef;
      TaGetPage (IdxF, IdxE, PrPgRef, &PagPtr);

      (*IdxE).Path[(*IdxE).PP].HasBits = 0;
      for (num = 0; num < (*PagPtr).ItemsOnPage; num++)
        (*IdxE).Path[(*IdxE).PP].HasBits +=
              (*PagPtr).ItemArray[num].DataRef;

      l = 0;
      r = (*PagPtr).ItemsOnPage - 1;
      do
        {
          k = ((l + r) / 2);
          c = TaCompKeys ((TaKeyStr *) ProcKey,
                          (*PagPtr).ItemArray[k].Key,
                          0,
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
      (*IdxE).Path[(*IdxE).PP].ItemArrIndex = r;
    }
  if (OKAY != T && (*IdxE).PP > -1)
    {
      while
        ((*IdxE).PP > 0 && (*IdxE).Path[(*IdxE).PP].ItemArrIndex == -1)
        (*IdxE).PP--;
      if ((*IdxE).Path[(*IdxE).PP].ItemArrIndex == -1)
        (*IdxE).PP = -1;
    }
}
/**************************************************************************/
void
FindKey (IdxF, IdxE, DataRecNum, ProcKey)

struct DataFile *IdxF;
struct IndexExt *IdxE;
unsigned long long *DataRecNum;
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
unsigned long long *DataRecNum;
void *ProcKey;
{
  void TaFindKey ();

  *DataRecNum = 0;
  TaFindKey (IdxF, IdxE, DataRecNum, ProcKey);
  if (OKAY != T)
    NextKey (IdxF, IdxE, DataRecNum, ProcKey);
}
/**************************************************************************/
