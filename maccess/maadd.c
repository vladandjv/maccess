/**************************************************************************/
/* PROGRAM: MAADD.C                                                       */
/* DESCRIPTION:  Adding record                                            */
/* AUTHOR: VLADAN DJORDJEVIC                                              */
/**************************************************************************/
#include "maccess.h"

Boolean PassUp; /* variables  */
TaPagePtr PagePtr1, PagePtr2;
struct TaItem ProcItem1, ProcItem2;
/**************************************************************************/
void
    AddRec(DatF, DatE, R, Buff)

        struct DataFile *DatF;
struct DataExt *DatE;
long long *R;
void *Buff;
{
#ifdef _HAS_FLUSH
  void PutFileHeader();
#endif /* _HAS_FLUSH */
  void NewRec();

  *R = 0;
  NewRec(DatF, DatE, R);
  PutRec(DatF, DatE, *R, Buff);
#ifdef _HAS_FLUSH
  PutFileHeader(DatF, DatE);
#endif /* _HAS_FLUSH */
}
/**************************************************************************/
void
    AddKey(IdxF, IdxE, DataRecNum, ProcKey)

        struct DataFile *IdxF;
struct IndexExt *IdxE;
long long *DataRecNum;
void *ProcKey;
{
#ifdef _HAS_FLUSH
  void StoreIndexHeader(), PutFileHeader();
#endif /* _HAS_FLUSH */
  void Search(), CutString(), TaNewPage(), TaUpdatePage();
  long long PrPgRef1;
  long long RBr = -1;

  if (strlen(*(TaKeyStr *)ProcKey) > (*IdxF).KeyL)
    CutString(*(TaKeyStr *)ProcKey, (*IdxF).KeyL);
  OKAY = T;

  Search((*IdxF).RR, IdxF, IdxE, DataRecNum, ProcKey, RBr);

  if (PassUp == T)
  {
    PrPgRef1 = (*IdxF).RR;
    TaNewPage(IdxF, IdxE, &((*IdxF).RR), &PagePtr1);
    (*PagePtr1).ItemsOnPage = 1;
    (*PagePtr1).BckwPageRef = PrPgRef1;
    (*PagePtr1).ItemArray[0] = ProcItem1;
    TaUpdatePage(PagePtr1);
  }
  (*IdxE).PP = -1;
#ifdef _HAS_FLUSH
  StoreIndexHeader(IdxF, IdxE);
  PutFileHeader(IdxF, &((*IdxE).DataE));
#endif /* _HAS_FLUSH */
}
/**************************************************************************/
void
    Search(PrPgRef1, IdxF, IdxE, DataRecNum, ProcKey, RBr)

        long long PrPgRef1;
struct DataFile *IdxF;
struct IndexExt *IdxE;
long long *DataRecNum;
void *ProcKey;
long long RBr;
{
  void TaGetPage(), Insert();
  long long TaCompKeys();
  long long k, l;
  long long c;

  if (PrPgRef1 == 0)
  {
    PassUp = T;
    strcpy(ProcItem1.Key, *(TaKeyStr *)ProcKey);
    ProcItem1.DataRef = *DataRecNum;
    ProcItem1.PageRef = 0;
  }
  else
  {
    TaGetPage(IdxF, IdxE, PrPgRef1, &PagePtr1);
    l = 0;
    RBr = (*PagePtr1).ItemsOnPage - 1;
    do
    {
      k = (long long)((l + RBr) / 2);
      c = TaCompKeys((TaKeyStr *)ProcKey,
                     (*PagePtr1).ItemArray[k].Key,
                     *DataRecNum,
                     (*PagePtr1).ItemArray[k].DataRef,
                     (*IdxE).AllowDuplKeys);
      if (c <= 0)
        RBr = k - 1;
      if (c >= 0)
        l = k + 1;
    } while (RBr >= l);
    if ((l - RBr) > 1)
    {
      OKAY = F;
      PassUp = F;
    }
    else
    {
      if (RBr == -1)
        Search((*PagePtr1).BckwPageRef, IdxF, IdxE, DataRecNum, ProcKey, RBr);
      else
        Search((*PagePtr1).ItemArray[RBr].PageRef, IdxF, IdxE,
               DataRecNum, ProcKey, RBr);
      if (PassUp == T)
        Insert(PrPgRef1, IdxF, IdxE, RBr);
    }
  }
}
/**************************************************************************/
void
    Insert(PrPgRef1, IdxF, IdxE, RBr)

        long long PrPgRef1;
struct DataFile *IdxF;
struct IndexExt *IdxE;
long long RBr;
{
  void TaNewPage(), TaUpdatePage(), TaGetPage();
  long long i;
  long long PrPgRef2;

  TaGetPage(IdxF, IdxE, PrPgRef1, &PagePtr1);
  if ((*PagePtr1).ItemsOnPage < PageSize)
  {
    (*PagePtr1).ItemsOnPage += 1;
    for (i = (*PagePtr1).ItemsOnPage - 1; i >= RBr + 2; i--)
      (*PagePtr1).ItemArray[i] = (*PagePtr1).ItemArray[i - 1];
    (*PagePtr1).ItemArray[RBr + 1] = ProcItem1;
    PassUp = F;
  }
  else
  {
    TaNewPage(IdxF, IdxE, &PrPgRef2, &PagePtr2);
    if (RBr <= Order - 1)
    {
      if (RBr == Order - 1)
        ProcItem2 = ProcItem1;
      else
      {
        ProcItem2 = (*PagePtr1).ItemArray[Order - 1];
        for (i = Order - 1; i >= RBr + 2; i--)
          (*PagePtr1).ItemArray[i] = (*PagePtr1).ItemArray[i - 1];
        (*PagePtr1).ItemArray[RBr + 1] = ProcItem1;
      }
      for (i = 0; i <= Order - 1; i++)
        (*PagePtr2).ItemArray[i] = (*PagePtr1).ItemArray[i + Order];
    }
    else
    {
      RBr = RBr - Order;
      ProcItem2 = (*PagePtr1).ItemArray[Order];
      for (i = 0; i <= RBr - 1; i++)
        (*PagePtr2).ItemArray[i] = (*PagePtr1).ItemArray[i + Order + 1];
      (*PagePtr2).ItemArray[RBr] = ProcItem1;
      for (i = RBr + 1; i <= Order - 1; i++)
        (*PagePtr2).ItemArray[i] = (*PagePtr1).ItemArray[i + Order];
    }
    (*PagePtr1).ItemsOnPage = Order;
    (*PagePtr2).ItemsOnPage = Order;
    (*PagePtr2).BckwPageRef = ProcItem2.PageRef;
    ProcItem2.PageRef = PrPgRef2;
    ProcItem1 = ProcItem2;
    TaUpdatePage(PagePtr2);
  }
  TaUpdatePage(PagePtr1);
}
/**************************************************************************/
