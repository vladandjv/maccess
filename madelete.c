/**************************************************************************/
/* PROGRAM: MADELETE.C                                                    */
/* DESCRIPTION:  Deleting record functions.                               */
/* AUTHOR: VLADAN DJORDJEVIC                                              */
/**************************************************************************/
#include "mainter.h"

Boolean PageTooSmall;
TaPagePtr PagPtrAB; /* communication between DelB i DelA */
long long k; /* communication betwen DelB i DelA */
/**************************************************************************/
void
DeleteRec (DatF, DatE, R)

struct DataFile *DatF;
struct DataExt *DatE;
long long R;
{
#ifdef _IMA_FLUSH
  void PutFileHeader ();
#endif /* _IMA_FLUSH */

  (*TaRecBuf).I = (*DatF).Heder.FirstFree;
  PutRec (DatF, DatE, R, TaRecBuf); /* PutRec should lock */
  (*DatF).Heder.FirstFree = R;
  (*DatF).Heder.NumberFree++;
#ifdef _IMA_FLUSH
  PutFileHeader (DatF, DatE);
#endif /* _IMA_FLUSH */
}
/**************************************************************************/
void
DelRecord (DatF, R)

struct DataFile *DatF;
long long R;
{
  void TaWriteRec ();

  (*TaRecBuf).I = (*DatF).Heder.FirstFree;
  TaWriteRec (DatF, R, TaRecBuf);
  (*DatF).Heder.FirstFree = R;
  (*DatF).Heder.NumberFree++;
}
/**************************************************************************/
void
TaReturnPage (PagPtr)

TaPagePtr PagPtr;
{
  void DelRecord ();

  DelRecord ((*(TaStackRecPtr) PagPtr).IndexFPtr,
             (*(TaStackRecPtr) PagPtr).PageRef);
  if (OKAY == F)
    {
      IOStatus = ErrReturnPage;
      TAIOCheck ();
    }
  (*(TaStackRecPtr) PagPtr).IndexFPtr = NULL;
  (*(TaStackRecPtr) PagPtr).IndexEPtr = NULL;
  (*(TaStackRecPtr) PagPtr).Updated = F;
}
/**************************************************************************/
void
DeleteKey (IdxF, IdxE, DataRecNum, ProcKey)

struct DataFile *IdxF;
struct IndexExt *IdxE;
long long *DataRecNum;
void *ProcKey;
{
#ifdef _IMA_FLUSH
  void StoreIndexHeader (), PutFileHeader ();
#endif /* _IMA_FLUSH */
  void TaGetPage (), CutString (), DelB ();
  TaPagePtr PagPtr;
  long long RBr = -1;

  if (strlen (*(TaKeyStr *) ProcKey) > (*IdxF).KeyL)
    CutString (*(TaKeyStr *) ProcKey, (*IdxF).KeyL);
  OKAY = T;
  DelB ((*IdxF).RR, IdxF, IdxE, DataRecNum, ProcKey, RBr);

  if (PageTooSmall == T)
    {
      TaGetPage (IdxF, IdxE, (*IdxF).RR, &PagPtr);
      if ((*PagPtr).ItemsOnPage == 0)
        {
          (*IdxF).RR = (*PagPtr).BckwPageRef;
          TaReturnPage (PagPtr);
        }
    }
  (*IdxE).PP = -1;
#ifdef _IMA_FLUSH
  StoreIndexHeader (IdxF, IdxE);
  PutFileHeader (IdxF, &((*IdxE).DataE));
#endif /* _IMA_FLUSH */
}
/**************************************************************************/
void
DelB (PrPgRef, IdxF, IdxE, DataRecNum, ProcKey, RBr)

long long PrPgRef;
struct DataFile *IdxF;
struct IndexExt *IdxE;
long long *DataRecNum;
void *ProcKey;
long long RBr;
{

  void TaGetPage (), TaUpdatePage (), UnderFlow (), DelA ();
  long long TaCompKeys ();
  long long i, l;
  long long c, XPageRef;

  if (PrPgRef == 0)
    {
      OKAY = F;
      PageTooSmall = F;
    }
  else
    {
      TaGetPage (IdxF, IdxE, PrPgRef, &PagPtrAB);
      l = 0;
      RBr = (*PagPtrAB).ItemsOnPage - 1;
      do
        {
          k = (long long) ((l + RBr) / 2);
          c = TaCompKeys ((TaKeyStr *) ProcKey,
                          (*PagPtrAB).ItemArray[k].Key,
                          *DataRecNum,
                          (*PagPtrAB).ItemArray[k].DataRef,
                          (*IdxE).AllowDuplKeys);
          if (c <= 0)
            RBr = k - 1;
          if (c >= 0)
            l = k + 1;
        }
      while (RBr >= l);
      if (RBr == -1)
        XPageRef = (*PagPtrAB).BckwPageRef;
      else
        XPageRef = (*PagPtrAB).ItemArray[RBr].PageRef;
      if (l - RBr > 1)
        {
          *DataRecNum = (*PagPtrAB).ItemArray[k].DataRef;
          if (XPageRef == 0)
            {
              (*PagPtrAB).ItemsOnPage--;
              if ((*PagPtrAB).ItemsOnPage < Order)
                PageTooSmall = T;
              else
                PageTooSmall = F;
              for (i = k; i < (*PagPtrAB).ItemsOnPage; i++)
                (*PagPtrAB).ItemArray[i] = (*PagPtrAB).ItemArray[i + 1];
              TaUpdatePage (PagPtrAB);
            }
          else
            {
              DelA (XPageRef, PrPgRef, IdxF, IdxE, DataRecNum, ProcKey);
              if (PageTooSmall == T)
                UnderFlow (PrPgRef, XPageRef, RBr, IdxF, IdxE);
            }
        }
      else
        {
          DelB (XPageRef, IdxF, IdxE, DataRecNum, ProcKey, RBr);
          if (PageTooSmall == T)
            UnderFlow (PrPgRef, XPageRef, RBr, IdxF, IdxE);
        }
    }
}
/**************************************************************************/
void
DelA (PrPgRef2, PrPgRef, IdxF, IdxE, DataRecNum, ProcKey)

long long PrPgRef2, PrPgRef;
struct DataFile *IdxF;
struct IndexExt *IdxE;
long long *DataRecNum;
void *ProcKey;
{

  void TaGetPage (), TaUpdatePage (), UnderFlow ();

  long long c;
  long long XPageRef;
  TaPagePtr PagePtr2;

  TaGetPage (IdxF, IdxE, PrPgRef2, &PagePtr2);
  XPageRef = (*PagePtr2).ItemArray[(*PagePtr2).ItemsOnPage - 1].PageRef;
  if (XPageRef != 0)
    {
      c = (*PagePtr2).ItemsOnPage - 1;
      DelA (XPageRef, PrPgRef, IdxF, IdxE, DataRecNum, ProcKey);
      if (PageTooSmall == T)
        UnderFlow (PrPgRef2, XPageRef, c, IdxF, IdxE);
    }
  else
    {
      TaGetPage (IdxF, IdxE, PrPgRef, &PagPtrAB);
      (*PagePtr2).ItemArray[(*PagePtr2).ItemsOnPage - 1].PageRef =
              (*PagPtrAB).ItemArray[k].PageRef;
      (*PagPtrAB).ItemArray[k] =
              (*PagePtr2).ItemArray[(*PagePtr2).ItemsOnPage - 1];
      (*PagePtr2).ItemsOnPage--;
      if ((*PagePtr2).ItemsOnPage < Order)
        PageTooSmall = T;
      else
        PageTooSmall = F;
      TaUpdatePage (PagPtrAB);
      TaUpdatePage (PagePtr2);
    }
}
/**************************************************************************/
void
UnderFlow (PrPgRef, PrPgRef2, r, IdxF, IdxE)

long long PrPgRef;
long long PrPgRef2;
long long r;
struct DataFile *IdxF;
struct IndexExt *IdxE;
{
  void TaGetPage (), TaUpdatePage ();
  long long i, k, LItem;
  long long LPageRef;
  TaPagePtr PagPtr, PagePtr2, l;

  TaGetPage (IdxF, IdxE, PrPgRef, &PagPtr);
  TaGetPage (IdxF, IdxE, PrPgRef2, &PagePtr2);
  if (r < (*PagPtr).ItemsOnPage - 1)
    {
      r++;
      LPageRef = (*PagPtr).ItemArray[r].PageRef;
      TaGetPage (IdxF, IdxE, LPageRef, &l);
      k = (long long) (((*l).ItemsOnPage - Order + 1) / 2) - 1;
      (*PagePtr2).ItemArray[Order - 1] = (*PagPtr).ItemArray[r];
      (*PagePtr2).ItemArray[Order - 1].PageRef = (*l).BckwPageRef;
      if (k > -1)
        {
          for (i = 0; i <= k - 1; i++)
            (*PagePtr2).ItemArray[i + Order] =
                  (*l).ItemArray[i];
          (*PagPtr).ItemArray[r] = (*l).ItemArray[k];
          (*PagPtr).ItemArray[r].PageRef = LPageRef;
          (*l).BckwPageRef = (*l).ItemArray[k].PageRef;
          (*l).ItemsOnPage = (*l).ItemsOnPage - k - 1;
          for (i = 0; i < (*l).ItemsOnPage; i++)
            (*l).ItemArray[i] = (*l).ItemArray[i + k + 1];
          (*PagePtr2).ItemsOnPage = Order + k;
          PageTooSmall = F;
          TaUpdatePage (l);
        }
      else
        {
          for (i = 0; i <= Order - 1; i++)
            (*PagePtr2).ItemArray[i + Order] = (*l).ItemArray[i];
          for (i = r; i < (*PagPtr).ItemsOnPage - 1; i++)
            (*PagPtr).ItemArray[i] =
                  (*PagPtr).ItemArray[i + 1];
          (*PagePtr2).ItemsOnPage = PageSize;
          (*PagPtr).ItemsOnPage = (*PagPtr).ItemsOnPage - 1;
          TaReturnPage (l);
          if ((*PagPtr).ItemsOnPage < Order)
            PageTooSmall = T;
          else
            PageTooSmall = F;
        }
      TaUpdatePage (PagePtr2);
    }
  else
    {
      if (r == 0)
        LPageRef = (*PagPtr).BckwPageRef;
      else
        LPageRef = (*PagPtr).ItemArray[r - 1].PageRef;
      TaGetPage (IdxF, IdxE, LPageRef, &l);
      LItem = (*l).ItemsOnPage + 1;
      k = (long long) ((LItem - Order) / 2) - 1;
      if (k > -1)
        {
          for (i = Order - 2; i >= 0; i--)
            (*PagePtr2).ItemArray[i + k + 1] =
                  (*PagePtr2).ItemArray[i];
          (*PagePtr2).ItemArray[k] = (*PagPtr).ItemArray[r];
          (*PagePtr2).ItemArray[k].PageRef = (*PagePtr2).BckwPageRef;
          LItem = LItem - k - 1;
          for (i = k - 1; i >= 0; i--)
            (*PagePtr2).ItemArray[i] = (*l).ItemArray[i + LItem];
          (*PagePtr2).BckwPageRef = (*l).ItemArray[LItem - 1].PageRef;
          (*PagPtr).ItemArray[r] = (*l).ItemArray[LItem - 1];
          (*PagPtr).ItemArray[r].PageRef = PrPgRef2;
          (*l).ItemsOnPage = LItem - 1;
          (*PagePtr2).ItemsOnPage = Order + k;
          PageTooSmall = F;
          TaUpdatePage (PagePtr2);
        }
      else
        {
          (*l).ItemArray[LItem - 1] = (*PagPtr).ItemArray[r];
          (*l).ItemArray[LItem - 1].PageRef = (*PagePtr2).BckwPageRef;
          for (i = 0; i <= Order - 2; i++)
            (*l).ItemArray[i + LItem] = (*PagePtr2).ItemArray[i];
          (*l).ItemsOnPage = PageSize;
          (*PagPtr).ItemsOnPage--;
          TaReturnPage (PagePtr2);
          if ((*PagPtr).ItemsOnPage < Order)
            PageTooSmall = T;
          else
            PageTooSmall = F;
        }
      TaUpdatePage (l);
    }
  TaUpdatePage (PagPtr);
}
/**************************************************************************/
