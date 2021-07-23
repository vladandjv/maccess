/**************************************************************************
 *  DATE.C
 *  Routines for date manipulations 
 *	Author: Vladan Djordjevic
 **************************************************************************/

#include "defs.h"
#include "time_date.h"
#include "stringv.h"

long long datesecond(char *InField);

char *
my_date(char *OutField)
{
  struct tm *dat;
  time_t timer;

  timer = time((time_t *)0);
  dat = localtime(&timer);
  dat->tm_mon += 1;
  dat->tm_year += 1900;
  sprintf(OutField, "%4.4d-%2.2d-%2.2d", dat->tm_year, dat->tm_mon, dat->tm_mday);
  return (OutField);
}
char *
datplus(char *OutField, char *InField, long long dani)
{
  struct tm *dat;
  time_t *tloc;

  tloc = (time_t *)malloc(sizeof(long long));
  *tloc = datesecond(InField);
  *tloc += (dani * 86400);
  dat = localtime(tloc);
  free(tloc);
  dat->tm_mon += 1;
  dat->tm_year += 1900;
  sprintf(OutField, "%4.4d-%2.2d-%2.2d", dat->tm_year, dat->tm_mon, dat->tm_mday);
  return (OutField);
}
char *
datrazl(char *OutField, char *InField1, char *InField2)
{
  long long prvi, drugi, razlika;

  prvi = datesecond(InField1);
  drugi = datesecond(InField2);
  if (prvi > drugi)
  {
    drugi *= (-1);
    prvi *= (-1);
  }
  razlika = (drugi - prvi) / 86400;
  sprintf(OutField, "%lld", razlika);
  return (OutField);
}
long long
datesecond(char *InField)
{
  long long brd, brm, brg, ukupno;
  long long b, cl;
  char pomoc[10], t[10];

  left(t, InField, 4);
  brg = atol(t);
  right(t, InField, 5);
  strcpy(pomoc, t);
  left(t, pomoc, 2);
  brm = atol(t);
  right(t, InField, 2);
  brd = atol(t);
  brd -= 1;
  brd *= 1440;
  brm -= 1;
  if ((brg - ((long long)(brg / 4)) * 4) == 0)
    switch (brm)
    {
    case 1:
      brm = 44640;
      break;
    case 2:
      brm = 86400;
      break;
    case 3:
      brm = 131040;
      break;
    case 4:
      brm = 174240;
      break;
    case 5:
      brm = 218880;
      break;
    case 6:
      brm = 262080;
      break;
    case 7:
      brm = 306720;
      break;
    case 8:
      brm = 351360;
      break;
    case 9:
      brm = 394560;
      break;
    case 10:
      brm = 439200;
      break;
    case 11:
      brm = 482400;
      break;
    case 12:
      brm = 527040;
      break;
    }
  else
    switch (brm)
    {
    case 1:
      brm = 44640;
      break;
    case 2:
      brm = 84960;
      break;
    case 3:
      brm = 129600;
      break;
    case 4:
      brm = 172800;
      break;
    case 5:
      brm = 217440;
      break;
    case 6:
      brm = 260640;
      break;
    case 7:
      brm = 305280;
      break;
    case 8:
      brm = 349920;
      break;
    case 9:
      brm = 393120;
      break;
    case 10:
      brm = 437760;
      break;
    case 11:
      brm = 480960;
      break;
    case 12:
      brm = 525600;
      break;
    }

  brg -= 1970;
  b = (long long)(brg / 4);
  cl = brg - ((long long)(brg / 4)) * 4;
  if (cl >= 2)
    b++;

  brg -= b;
  brg *= 525600;
  brg += (b * 527040);
  ukupno = brd + brm + brg;

  ukupno *= 60;
  return (ukupno);
}

char *
okrenidat(char *OutField, char *InField)
{
  *(OutField + 0) = *(InField + 8);
  *(OutField + 1) = *(InField + 9);
  *(OutField + 2) = '-';
  *(OutField + 3) = *(InField + 5);
  *(OutField + 4) = *(InField + 6);
  *(OutField + 5) = '-';
  *(OutField + 6) = *(InField + 0);
  *(OutField + 7) = *(InField + 1);
  *(OutField + 8) = *(InField + 2);
  *(OutField + 9) = *(InField + 3);
  *(OutField + 10) = '-';
  *(OutField + 11) = '\0';
  return (OutField);
}
