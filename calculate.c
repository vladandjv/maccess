/**************************************************************************/
/* PROGRAM: CALCULATE.C                                                   */
/* DESCRIPTION: CALCULATES THE VALUES OF THE B + VERSION OF THE BINARY    */
/* TREE AND DETERMINES THE NEEDED RESOURCES OF THE SYSTEM. ALSO,          */
/* DETERMINES THE OPTIMAL EFFICIENCY - RESOURCE CONSUMPTION RATIO.        */
/* AUTHOR: VLADAN DJORDJEVIC   		                                  */
/**************************************************************************/
#include "screen.h"
#include <math.h>

int MaxMaxHeight;
/**************************************************************************/
int
main ()
{
  void RacunajHeight ();
  Boolean prov0 (), prov1 (), prov2 (), prov3 (), prov4 ();
  char slogovi[9], velslog[6], kljuc[4], strana[5], stackst[5], gustina[4];
  int Cit, MaxDataRecSize, MaxKeyLen, Order, I;
  double PageSize, PageStackSize, MaxRecs, DxSize, IxSize, MaxSearch, StackMem;
  double MemSearch, DiskSearch, PerPage, TotalPages, IrSize, M, Temp, NumRecs;
  double Density; /* degree of file occupancy */

  InitEkran ();
  clear ();
  Frame (0, 0, 3, 78);
  mvaddstr (1, 1, "CALCULATION OF DATABASE EFFICIENCY AND RESOURCES REQUIREMENTS BASED ON THE");
  mvaddstr (2, 1, "PARAMETERS ENTERED FOR B+ VERSION OF BINARY TREE. AUTHOR: VLADAN DJORDJEVIC");
  mvaddstr (4, 25, "FOR HELP PRESS CTRL-i!");
  Frame (5, 0, 22, 78);
  Line (12, 0, 78);
  Line (15, 0, 78);
  mvaddstr (13, 1, "PARAMETAR Order SHOULD BE:");
  mvaddstr (14, 1, "PARAMETAR Height SHOULD BE:");
  mvaddstr (16, 1, "SIZE OF DATA FILE:");
  mvaddstr (17, 1, "SIZE OF INDEX FILE:");
  mvaddstr (18, 1, "MEMORY NEEDED FOR Page Stack:");
  mvaddstr (19, 1, "AVERAGE NUMBER OF COMPARISONS IN KEY SEARCH:");
  mvaddstr (20, 1, "PERCENTAGE OF ONLY MEMORY SEARCH:");
  mvaddstr (21, 1, "PERCENTAGE OF REQUIRED DISK SEARCH:");
  strcpy (slogovi, "150000  ");
  strcpy (gustina, "75 ");
  strcpy (velslog, "2446 ");
  strcpy (kljuc, "44 ");
  strcpy (strana, "60 ");
  strcpy (stackst, "45 ");
  for (;;)
    {
      GetField ("ASSUMED TOTAL NUMBER OF RECORDS IN THE DATA FILE:", 6, 1, slogovi, 6, 8, 8, T, prov0);
      GetField ("ASSUMED COMPLETENESS OF FILES WITH RECORDS (OPTIMUM 75%):", 7, 1, gustina, 7, 3, 3, T, prov2);
      GetField ("MAXIMUM SIZE OF THE RECORD:                                      ", 8, 1, velslog, 8, 5, 5, T, prov0);
      GetField ("MAXIMUM LENGTH OF THE KEY:                                        ", 9, 1, kljuc, 9, 3, 3, T, prov1);
      GetField ("MAXIMUM NUMBER OF THE KEYS IN THE PAGE - Page Size:              ", 10, 1, strana, 10, 4, 4, T, prov3);
      GetField ("MAXIMUM NUMBER OF THE PAGES IN MEMORY - Page Stack Size:         ", 11, 1, stackst, 11, 4, 4, T, prov4);
      Cit = Read ();
      if (Cit == -1 || Cit == -2)
        break;
      MaxRecs = atof (slogovi);
      Density = atof (gustina) / 100;
      MaxDataRecSize = atoi (velslog);
      MaxKeyLen = atoi (kljuc);
      PageSize = atof (strana);
      PageStackSize = atof (stackst);

      Order = (int) (PageSize / 2);
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
      RacunajHeight (PageSize, MaxRecs);
      mvprintw (13, 61, "%10.0d", Order);
      mvprintw (14, 61, "%10.0d", MaxMaxHeight);
      mvprintw (16, 58, "%13.0f bytes", DxSize);
      mvprintw (17, 59, "%12.0f bytes", IxSize);
      mvprintw (18, 61, "%10.0f bytes", StackMem);
      mvprintw (19, 61, "%10.2f", MaxSearch);
      mvprintw (20, 61, "%10.2f%c", MemSearch, '\x25');
      mvprintw (21, 61, "%10.2f%c", DiskSearch, '\x25');
      refresh ();
    }
  TermEkran ();
  return 0;
}
/**************************************************************************/
void
RacunajHeight (PageSize, MaxRecs)

double PageSize;
double MaxRecs;
{
  int MaxHeight, i;
  double MaxSearch, PerPage, NumRecs;

  MaxMaxHeight = 0;
  for (i = 2; i <= 4; i++)
    {
      PerPage = PageSize * (i * 0.25);
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
Boolean
prov0 (field)

char *field;
{
  unsigned int i;
  char intermediate[10];

  if (isEmpty (field) == T)
    {
      message ("Mandatory field!");
      return (F);
    }
  strcpy (intermediate, ltrim (field));
  strcpy (intermediate, trim (intermediate));
  for (i = 0; i < strlen (intermediate); i++)
    if (!isdigit (intermediate[i]))
      {
        message ("Only digits!");
        return (F);
      }
  if (atoi (intermediate) <= 0)
    {
      message ("Must be greater then 0!");
      return (F);
    }
  return (T);
}
/**************************************************************************/
Boolean
prov1 (field)

char *field;
{
  unsigned int i;
  long j;
  char intermediate[10];

  if (isEmpty (field) == T)
    {
      message ("Mandatory field!");
      return (F);
    }
  strcpy (intermediate, ltrim (field));
  strcpy (intermediate, trim (intermediate));
  for (i = 0; i < strlen (intermediate); i++)
    if (!isdigit (intermediate[i]))
      {
        message ("Only digits!");
        return (F);
      }
  j = atoi (intermediate);
  if (j < 1 || j > 512)
    {
      message ("The range is from 1 to 512!");
      return (F);
    }
  return (T);
}
/**************************************************************************/
Boolean
prov2 (field)

char *field;
{
  unsigned int i;
  long j;
  char intermediate[5];

  if (isEmpty (field) == T)
    {
      message ("Mandatory field!");
      return (F);
    }
  strcpy (intermediate, ltrim (field));
  strcpy (intermediate, trim (intermediate));
  for (i = 0; i < strlen (intermediate); i++)
    if (!isdigit (intermediate[i]))
      {
        message ("Only digits!");
        return (F);
      }
  j = atoi (intermediate);
  if (j < 1 || j > 100)
    {
      message ("The range is from 1 to 100!");
      return (F);
    }
  return (T);
}
/**************************************************************************/
Boolean
prov3 (field)

char *field;
{
  unsigned int i;
  double j;
  char intermediate[5];

  if (isEmpty (field) == T)
    {
      message ("Mandatory field!");
      return (F);
    }
  strcpy (intermediate, ltrim (field));
  strcpy (intermediate, trim (intermediate));
  for (i = 0; i < strlen (intermediate); i++)
    if (!isdigit (intermediate[i]))
      {
        message ("Only digits!");
        return (F);
      }
  j = atof (intermediate);
  if (j < 4 || fmod (j, 2) != 0)
    {
      message ("Must at least 4 and even number!");
      return (F);
    }
  return (T);
}
/**************************************************************************/
Boolean
prov4 (field)

char *field;
{
  unsigned int i;
  long j;
  char intermediate[5];

  if (isEmpty (field) == T)
    {
      message ("Mandatory field!");
      return (F);
    }
  strcpy (intermediate, ltrim (field));
  strcpy (intermediate, trim (intermediate));
  for (i = 0; i < strlen (intermediate); i++)
    if (!isdigit (intermediate[i]))
      {
        message ("Only digits!");
        return (F);
      }
  j = atoi (intermediate);
  if (j < 3)
    {
      message ("Must at least 3!");
      return (F);
    }
  return (T);
}
/**************************************************************************/
void
Help (Headline)

char *Headline;
{
  savescreen ();
  ShowSymbols ();
  restscreen ();
}
/**************************************************************************/
void
Help1 ()
{
  int i = 0;

  i++;
}
/**************************************************************************/
