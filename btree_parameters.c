/*
 *    Filename:  btree_parameters.c
 *    Author: Vladan Djordjevic
 *
 *    Description: Displays current parameters, calculates parameters 
 *    which are interdependent and also calculates shared memory segment 
 *    size with this formula: 
 *    MACCESS_SHM_MEM_SIZE = (PageSize * sizeof(struct DataFile) + 
 *    sizeof(TaPageStack) + sizeof(TaPageMap) + sizeof(long long)); 
 */

#include <stdio.h>
#include "mainter.h"
#include "maccess.h"
#include <math.h>

int MaxMaxHeight; /* Needed for as result for CalculateHeight() */

/* Undefining parameters in maccess.h */
#undef FileHeaderSize
#undef ItemOverhead
#undef PageOverhead
#undef Order
#undef MaxHeight

#define FileHeaderSize (sizeof(long long)*3 + sizeof(long long)) 
#define ItemOverhead sizeof(struct TaItem) - MaxKeyLen
#define PageOverhead (sizeof(int)+sizeof(long long))
/**************************************************************************/
int
main ()
{
  void CalculateHeight ();
  long long size;
  int Order, I;
  double MaxRecs, PgSize, DxSize, IxSize, MaxSearch, StackMem;
  double MemSearch, DiskSearch, PerPage, TotalPages, IrSize, M, Temp, NumRecs;
  double Density; /* degree of file occupancy */

  PgSize = (int) PageSize;
  MaxRecs = atof ("100000000"); /* Assumed total number of records in 
                                in a single datafile.*/
  Density = atof ("75") / 100; /* Assumed completeness of files with 
				   records (optimum 75%) */

  Order = (int) (PgSize / 2);
  CalculateHeight (PgSize, MaxRecs); /* Calculates MaxMaxHeight */

  PerPage = PageSize*Density;
  if (MaxRecs < 1000)
    NumRecs = 1000;
  else
    NumRecs = MaxRecs;
  MaxSearch = log (NumRecs) / log (PerPage);
  TotalPages = (int) (NumRecs / PerPage + 1);
  Temp = 1;
  M = PerPage;
  I = 1;
  while (Temp + M < PageStackSize)
    {
      Temp = Temp + M;
      I = I + 1;
      M = exp (log (PerPage) * I);
    }
  if (Temp + M > TotalPages)
    M = TotalPages - Temp + 1;
  MemSearch = I + (PageStackSize - Temp) / M;
  DiskSearch = MaxSearch - MemSearch;
  IrSize = (MaxKeyLen + 9) * PageSize + 5;
  IxSize = IrSize*TotalPages;
  DxSize = MaxDataRecSize * (MaxRecs + 1);
  StackMem = IrSize * PageStackSize;
  MemSearch = (MemSearch / MaxSearch) * 100;
  DiskSearch = (DiskSearch / MaxSearch) * 100;
  if (MemSearch > 100.0)
    {
      MemSearch = 99.00;
      DiskSearch = 1.00;
    }

  printf ("B+ tree current PARAMETERS are:\n");
  printf ("MaxKeyLen              = %d\n", MaxKeyLen);
  printf ("FileHeaderSize         = %ld\n", ((sizeof (long long)*3 + \
					sizeof (long long))));
  printf ("PageSize               = %3d - The range is from 4 to 254\n", PageSize);
  printf ("PageStackSize          = %3d - The range is from 3 to 255\n", PageStackSize);
  printf ("ItemOverhead           = %ld\n", (sizeof (struct TaItem) -MaxKeyLen));
  printf ("PageOverhead           = %ld\n", (sizeof (long long) + \
			  sizeof (long long)));
  printf ("Maximum MaxDataRecSize = %ld\n", (PageOverhead + \
				PageSize * (ItemOverhead + MaxKeyLen)));
  printf ("Acctual MaxDataRecSize = %d\n\n", MaxDataRecSize);

  printf ("Size of TYPES are:\n");
  printf ("SizeOf struct DataFile = %ld\n", sizeof (struct DataFile));
  printf ("SizeOf struct DataExt  = %ld\n", sizeof (struct DataFile));
  printf ("SizeOf struct IndexExt = %ld\n", sizeof (struct DataFile));
  printf ("SizeOf TaPageStack     = %ld\n", sizeof (TaPageStack));
  printf ("SizeOf TaPageMap       = %ld\n", sizeof (TaPageMap));
  printf ("SizeOf long long       = %ld\n\n", sizeof (long long));

  printf ("RESULTS for the assumed total number of 100 milion records\nin a single datafile:\n\n");
  printf ("Size of data file:                        %15.0f bytes\n", DxSize);
  printf ("Size of index file:                       %15.0f bytes\n", IxSize);
  printf ("Memory needed for Page Stack:             %15.0f bytes\n", StackMem);
  printf ("Average number of comparisions in search: %3.2f\n", MaxSearch);
  printf ("Assumed Density (100%% = no deleted rec.): %3.2f%c\n", Density, '\x25');
  printf ("Percentage of only memory search:        %3.2f%c\n", MemSearch, '\x25');
  printf ("Percentage of required disk search:      %3.2f%c\n\n", DiskSearch, '\x25');

  size = ((PageSize * sizeof (struct DataFile)) + sizeof (TaPageStack) + \
			sizeof (TaPageMap) + sizeof (long long));
  printf ("MACCESS_SHM_MEM_SIZE should be = %lld bytes\n", size);
  printf ("Order should be                = %d\n", Order);
  printf ("Height should be               = %d\n\n", MaxMaxHeight);
  printf ("Please COMPARE this \"should be\" RESULTS with \nthe parametars in includes.h and maccess.h!!!\n");

  exit (0);
}
/**************************************************************************/
void
CalculateHeight (PgSize, MaxRecs)

double PgSize;
double MaxRecs;
{
  int MaxHeight, i;
  double MaxSearch, PerPage, NumRecs;

  MaxMaxHeight = 0;
  for (i = 2; i <= 4; i++)
    {
      PerPage = PgSize * (i * 0.25);
      if (MaxRecs < 1000)
        NumRecs = 1000;
      else
        NumRecs = MaxRecs;
      MaxSearch = log (NumRecs) / log (PerPage);
      MaxHeight = (int) (MaxSearch + 3); /*the original is one, but 2 or 3 
                                           * are recommended for safety */
      if (MaxHeight > MaxMaxHeight)
        MaxMaxHeight = MaxHeight;
    }
}
/**************************************************************************/

