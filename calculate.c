/**************************************************************************/
/* PROGRAM: CALCULATE.C                                                   */
/* DESCRIPTION: CALCULATES THE VALUES OF THE B+ VERSION OF THE BINARY     */
/* TREE AND DETERMINES THE NEEDED RESOURCES OF THE SYSTEM. ALSO,          */
/* DETERMINES THE OPTIMAL EFFICIENCY - RESOURCE CONSUMPTION RATIO.        */
/* AUTHOR: VLADAN DJORDJEVIC   		                                        */
/**************************************************************************/
#include "screen.h"
#include <math.h>

int MaxMaxHeight;
/**************************************************************************/
int main()
{
  void CalculateHeight();
  Boolean check0(), check1(), check2(), check3(), check4();
  char records[9], recordsize[6], key[4], page[5], stackst[5], denseness[4];
  int Cit, MaxDataRecSizeV, MaxKeyLenV, OrderV, I;
  double PageSizeV, MaxRecs, DxSize, IxSize, MaxSearch, StackMem;
  double MemSearch, DiskSearch, PerPage, TotalPages, IrSize, M, Temp, NumRecs;
  double Density; /* degree of file occupancy */
  long long ShmSize, PageStackSizeV;

  InitEkran();
  clear();
  Frame(0, 0, 3, 78);
  mvaddstr(1, 1, "CALCULATION OF DATABASE EFFICIENCY AND RESOURCES REQUIREMENTS BASED ON THE");
  mvaddstr(2, 1, "PARAMETERS ENTERED FOR B+ VERSION OF BINARY TREE. AUTHOR: VLADAN DJORDJEVIC");
  mvaddstr(4, 25, "FOR HELP PRESS CTRL-i!");
  Frame(5, 0, 23, 78);
  Line(12, 0, 78);
  Line(15, 0, 78);
  mvaddstr(13, 1, "PARAMETAR Order SHOULD BE:");
  mvaddstr(14, 1, "PARAMETAR Height SHOULD BE:");
  mvaddstr(16, 1, "SIZE OF DATA FILE:");
  mvaddstr(17, 1, "SIZE OF INDEX FILE:");
  mvaddstr(18, 1, "MEMORY NEEDED FOR Page Stack:");
  mvaddstr(19, 1, "SHM SIZE:");
  mvaddstr(20, 1, "AVERAGE NUMBER OF COMPARISONS IN KEY SEARCH:");
  mvaddstr(21, 1, "PERCENTAGE OF ONLY MEMORY SEARCH:");
  mvaddstr(22, 1, "PERCENTAGE OF REQUIRED DISK SEARCH:");
  strcpy(records, "10000000");
  strcpy(denseness, "75 ");
  strcpy(recordsize, "150  ");
  strcpy(key, "35 ");
  strcpy(page, "1016");
  strcpy(stackst, "1020");
  for (;;)
  {
    GetField("ASSUMED TOTAL NUMBER OF RECORDS IN THE DATA FILE:", 6, 1, records, 6, 8, 8, T, check0);
    GetField("ASSUMED COMPLETENESS OF FILES WITH RECORDS (OPTIMUM 75%):", 7, 1, denseness, 7, 3, 3, T, check2);
    GetField("MAXIMUM SIZE OF THE RECORD:                                      ", 8, 1, recordsize, 8, 5, 5, T, check0);
    GetField("MAXIMUM LENGTH OF THE KEY:                                       ", 9, 1, key, 9, 3, 3, T, check1);
    GetField("MAXIMUM NUMBER OF THE KEYS IN THE PAGE - Page Size:              ", 10, 1, page, 10, 4, 4, T, check3);
    GetField("MAXIMUM NUMBER OF THE PAGES IN MEMORY - Page Stack Size:         ", 11, 1, stackst, 11, 4, 4, T, check4);
    Cit = Read();
    if (Cit == -1 || Cit == -2)
      break;
    MaxRecs = atof(records);
    Density = atof(denseness) / 100;
    MaxDataRecSizeV = atoi(recordsize);
    MaxKeyLenV = atoi(key);
    PageSizeV = atof(page);
    PageStackSizeV = atol(stackst);

    OrderV = (int)(PageSizeV / 2);
    PerPage = PageSizeV * Density;
    if (MaxRecs < 1000)
      NumRecs = 1000;
    else
      NumRecs = MaxRecs;
    MaxSearch = log(NumRecs) / log(PerPage);
    TotalPages = (int)(NumRecs / PerPage + 1);
    Temp = 1;
    M = PerPage;
    I = 1;
    while (Temp + M < PageStackSizeV)
    {
      Temp = Temp + M;
      I = I + 1;
      M = exp(log(PerPage) * I);
    }
    if (Temp + M > TotalPages)
      M = TotalPages - Temp + 1;
    MemSearch = I + (PageStackSizeV - Temp) / M;
    DiskSearch = MaxSearch - MemSearch;
    IrSize = (MaxKeyLenV + 9) * PageSizeV + 5;
    IxSize = IrSize * TotalPages;
    DxSize = MaxDataRecSizeV * (MaxRecs + 1);
    StackMem = IrSize * PageStackSizeV;
    MemSearch = (MemSearch / MaxSearch) * 100;
    DiskSearch = (DiskSearch / MaxSearch) * 100;
    if (MemSearch > 100.0)
    {
      MemSearch = 99.00;
      DiskSearch = 1.00;
    }
    ShmSize = ((PageSizeV * sizeof(struct DataFile)) +
               (sizeof(struct TaStackRec) * PageStackSizeV) +
               (sizeof(long long) * PageStackSizeV));
    CalculateHeight(PageSizeV, MaxRecs);
    mvprintw(13, 61, "%10.0d", OrderV);
    mvprintw(14, 61, "%10.0d", MaxMaxHeight);
    mvprintw(16, 58, "%13.0f bytes", DxSize);
    mvprintw(17, 59, "%12.0f bytes", IxSize);
    mvprintw(18, 61, "%10.0f bytes", StackMem);
    mvprintw(19, 61, "%10.0lld bytes", ShmSize);
    mvprintw(20, 61, "%10.2f", MaxSearch);
    mvprintw(21, 61, "%10.2f%c", MemSearch, '\x25');
    mvprintw(22, 61, "%10.2f%c", DiskSearch, '\x25');
    refresh();
  }
  TermEkran();
  return 0;
}
/**************************************************************************/
void
    CalculateHeight(PageSizeV, MaxRecs)

        double PageSizeV;
double MaxRecs;
{
  int MaxHeightV, i;
  double MaxSearch, PerPage, NumRecs;

  MaxMaxHeight = 0;
  for (i = 2; i <= 4; i++)
  {
    PerPage = PageSizeV * (i * 0.25);
    if (MaxRecs < 1000)
      NumRecs = 1000;
    else
      NumRecs = MaxRecs;
    MaxSearch = log(NumRecs) / log(PerPage);
    MaxHeightV = (int)(MaxSearch + 3); /*the original is one, but 2 or 3 
                                           * are recommended for safety */
    if (MaxHeightV > MaxMaxHeight)
      MaxMaxHeight = MaxHeightV;
  }
}
/**************************************************************************/
Boolean
    check0(field)

        char *field;
{
  unsigned int i;
  char intermediate[10], out[10];

  if (isEmpty(field) == T)
  {
    message("Mandatory field!");
    return (F);
  }
  strcpy(intermediate, ltrim(out, field));
  strcpy(intermediate, trim(out, intermediate));
  for (i = 0; i < strlen(intermediate); i++)
    if (!isdigit(intermediate[i]))
    {
      message("Only digits!");
      return (F);
    }
  if (atoi(intermediate) <= 0)
  {
    message("Must be greater then 0!");
    return (F);
  }
  return (T);
}
/**************************************************************************/
Boolean
    check1(field)

        char *field;
{
  unsigned int i;
  long j;
  char intermediate[10], out[10];

  if (isEmpty(field) == T)
  {
    message("Mandatory field!");
    return (F);
  }
  strcpy(intermediate, ltrim(out, field));
  strcpy(intermediate, trim(out, intermediate));
  for (i = 0; i < strlen(intermediate); i++)
    if (!isdigit(intermediate[i]))
    {
      message("Only digits!");
      return (F);
    }
  j = atoi(intermediate);
  if (j < 1 || j > 512)
  {
    message("The range is from 1 to 512!");
    return (F);
  }
  return (T);
}
/**************************************************************************/
Boolean
    check2(field)

        char *field;
{
  unsigned int i;
  long j;
  char intermediate[5], out[5];

  if (isEmpty(field) == T)
  {
    message("Mandatory field!");
    return (F);
  }
  strcpy(intermediate, ltrim(out, field));
  strcpy(intermediate, trim(out, intermediate));
  for (i = 0; i < strlen(intermediate); i++)
    if (!isdigit(intermediate[i]))
    {
      message("Only digits!");
      return (F);
    }
  j = atoi(intermediate);
  if (j < 1 || j > 100)
  {
    message("The range is from 1 to 100!");
    return (F);
  }
  return (T);
}
/**************************************************************************/
Boolean
    check3(field)

        char *field;
{
  unsigned int i;
  double j;
  char intermediate[5], out[5];

  if (isEmpty(field) == T)
  {
    message("Mandatory field!");
    return (F);
  }
  strcpy(intermediate, ltrim(out, field));
  strcpy(intermediate, trim(out, intermediate));
  for (i = 0; i < strlen(intermediate); i++)
    if (!isdigit(intermediate[i]))
    {
      message("Only digits!");
      return (F);
    }
  j = atof(intermediate);
  if (j < 4 || fmod(j, 2) != 0)
  {
    message("Must at least 4 and even number!");
    return (F);
  }
  return (T);
}
/**************************************************************************/
Boolean
    check4(field)

        char *field;
{
  unsigned int i;
  long j;
  char intermediate[5], out[5];

  if (isEmpty(field) == T)
  {
    message("Mandatory field!");
    return (F);
  }
  strcpy(intermediate, ltrim(out, field));
  strcpy(intermediate, trim(out, intermediate));
  for (i = 0; i < strlen(intermediate); i++)
    if (!isdigit(intermediate[i]))
    {
      message("Only digits!");
      return (F);
    }
  j = atoi(intermediate);
  if (j < 3)
  {
    message("Must at least 3!");
    return (F);
  }
  return (T);
}
/**************************************************************************/
void
    Help(Headline)

        char *Headline;
{
  savescreen();
  ShowSymbols();
  restscreen();
}
/**************************************************************************/
