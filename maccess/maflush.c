/**************************************************************************/
/* PROGRAM: MAFLUSH.C                                                     */
/* DESCRIPTION:  Flush from memory to the files on disk.                  */
/* AUTHOR: VLADAN DJORDJEVIC                                              */
/**************************************************************************/
#include "maccess.h"
/**************************************************************************/
void
    FlushFile(DatF, DatE)

        struct DataFile *DatF;
struct DataExt *DatE;
{
  void PutFileHeader();
  long long clone;

  PutFileHeader(DatF, DatE);
  clone = dup((*DatE).FileNumber);
  if (close(clone) == -1)
  {
    IOStatus = ErrFlushFile;
    TAIOCheck();
  }
  else
    OKAY = T;
}
/**************************************************************************/
void
    FlushIndex(IdxF, IdxE)

        struct DataFile *IdxF;
struct IndexExt *IdxE;
{
  void StoreIndexHeader();
  void PutFileHeader();
  long long clone;

  StoreIndexHeader(IdxF, IdxE);
  PutFileHeader(IdxF, &((*IdxE).DataE));
  clone = dup((*IdxE).DataE.FileNumber);
  if (close(clone) == -1)
  {
    IOStatus = ErrFlushFile;
    TAIOCheck();
  }
  else
    OKAY = T;
}
/**************************************************************************/
