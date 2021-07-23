/*
 *    Filename:  btree_parameters.c
 *
 *    Description: Displays current parameters, calculates parameters 
 *    which are interdependent.
 * 
 *    Author: Vladan Djordjevic
 */

#include "maccess.h"
#include <stdio.h>
#include <math.h>

int MaxMaxHeight; /* Needed for as result for CalculateHeight() */

/* Undefining parameters in maccess.h */
#undef FileHeaderSize
#undef ItemOverhead
#undef PageOverhead
#undef Order
#undef MaxHeight

#define FileHeaderSize (sizeof(long long) * 3 + sizeof(long long))
#define ItemOverhead (sizeof(long long) * 2)
#define PageOverhead (sizeof(((struct TaPage *)0)->ItemsOnPage) + sizeof(((struct TaPage *)0)->BckwPageRef))

/**************************************************************************/
int main()
{
	void CalculateHeight();
	long long size;
	int Order, I;
	double MaxRecs, PgSize, DxSize, IxSize, MaxSearch, StackMem;
	double MemSearch, DiskSearch, PerPage, TotalPages, IrSize, M, Temp, NumRecs;
	double Density; /* degree of file occupancy */

	PgSize = (long long)PageSize;
	MaxRecs = atof("10000000"); /* Assumed total number of records in 
					in a single datafile.*/
	Density = atof("75") / 100; /* Assumed completeness of files with 
					records (optimum 75%) */

	Order = (int)(PgSize / 2);
	CalculateHeight(PgSize, MaxRecs); /* Calculates MaxMaxHeight */

	PerPage = PageSize * Density;
	if (MaxRecs < 1000)
		NumRecs = 1000;
	else
		NumRecs = MaxRecs;
	MaxSearch = log(NumRecs) / log(PerPage);
	TotalPages = (int)(NumRecs / PerPage + 1);
	Temp = 1;
	M = PerPage;
	I = 1;
	while (Temp + M < PageStackSize)
	{
		Temp = Temp + M;
		I = I + 1;
		M = exp(log(PerPage) * I);
	}
	if (Temp + M > TotalPages)
		M = TotalPages - Temp + 1;
	MemSearch = I + (PageStackSize - Temp) / M;
	DiskSearch = MaxSearch - MemSearch;
	IrSize = (MaxKeyLen + 9) * PageSize + 5;
	IxSize = IrSize * TotalPages;
	DxSize = MaxDataRecSize * (MaxRecs + 1);
	StackMem = IrSize * PageStackSize;
	MemSearch = (MemSearch / MaxSearch) * 100;
	DiskSearch = (DiskSearch / MaxSearch) * 100;
	if (MemSearch > 100.0)
	{
		MemSearch = 99.00;
		DiskSearch = 1.00;
	}

	printf("B+ tree current PARAMETERS are:\n");
	printf("MaxKeyLen              = %lld\n", MaxKeyLen);
	printf("PageSize               = %3lld - Must be at least 4!\n", PageSize);
	printf("PageStackSize          = %3lld - Must be at least 3!\n", PageStackSize);
	printf("Order                  = %d\n", Order);
	printf("FileHeaderSize         = %zu\n", ((sizeof(long long) * 3 +
						   sizeof(long long))));
	printf("MinDataRecSize         = %zu (same as FileHeaderSize)\n",
	       ((sizeof(long long) * 3 +
		 sizeof(long long))));
	printf("ItemOverhead           = %llu\n", (sizeof(struct TaItem) - MaxKeyLen));
	printf("PageOverhead           = %zu\n", (sizeof(((struct TaPage *)0)->ItemsOnPage) +
						  sizeof(((struct TaPage *)0)->BckwPageRef)));
	printf("Maximum MaxDataRecSize = %llu\n", (PageOverhead + PageSize * (ItemOverhead + MaxKeyLen)));
	printf("Acctual MaxDataRecSize = %lld\n\n", MaxDataRecSize);

	printf("Size of TYPES are:\n");
	printf("SizeOf struct DataFile = %zu\n", sizeof(struct DataFile));
	printf("SizeOf struct DataExt  = %zu\n", sizeof(struct DataFile));
	printf("SizeOf struct IndexExt = %zu\n", sizeof(struct DataFile));
	printf("SizeOf TaPageStack     = %zu\n", sizeof(TaPageStack));
	printf("SizeOf TaPageMap       = %zu\n", sizeof(TaPageMap));
	printf("SizeOf long long       = %zu\n\n", sizeof(long long));
	printf("RESULTS for the assumed total number of 10 milion records\nin a single datafile:\n\n");
	printf("Size of data file:                        %15.0f bytes\n", DxSize);
	printf("Size of index file:                       %15.0f bytes\n", IxSize);
	printf("Memory needed for Page Stack:             %15.0f bytes\n", StackMem);
	printf("Average number of comparisions in search: %3.2f\n", MaxSearch);
	printf("Assumed Density (100%% = no deleted rec.): %3.2f%c\n", Density, '\x25');
	printf("Percentage of only memory search:        %3.2f%c\n", MemSearch, '\x25');
	printf("Percentage of required disk search:      %3.2f%c\n\n", DiskSearch, '\x25');

	size = ((PageSize * sizeof(struct DataFile)) + sizeof(TaPageStack) +
		sizeof(TaPageMap));
	printf("MACCESS_SHM_MEM_SIZE = %lld bytes\n", size);
	printf("Height SHOULD BE               = %d (!!!)\n\n", MaxMaxHeight);
	printf("Please COMPARE this Height result with the parameter in maccess.h!\n You can change the parameters according to your requests\n and improve performance, but first check the theory\nin the documentation to be sure what the parameters mean and how\nthey are connected. After changing the parameters, compile again\nthis file with the new parameters, run it and compare!!!\n");

	exit(0);
}
/**************************************************************************/
void
    CalculateHeight(PgSize, MaxRecs)

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
		MaxSearch = log(NumRecs) / log(PerPage);
		MaxHeight = (int)(MaxSearch + 3); /*the original is one, but 2 or 3 
						                                               * are recommended for safety */
		if (MaxHeight > MaxMaxHeight)
			MaxMaxHeight = MaxHeight;
	}
}
/**************************************************************************/
