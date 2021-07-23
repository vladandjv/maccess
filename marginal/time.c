/**************************************************************************
 *  TIME.C
 *  Routines for date manipulations 
 *	Author: Vladan Djordjevic
 **************************************************************************/
#include "defs.h"
#include "time_date.h"
#include "stringv.h"

long long timesecond(char *InField);

char *
my_time(char *OutField)
{
  struct tm *dat;
  time_t timer;

  timer = time((time_t *)0);
  dat = localtime(&timer);
  sprintf(OutField, "%2.2d:%2.2d:%2.2d", dat->tm_hour, dat->tm_min, dat->tm_sec);
  return (OutField);
}
char *
time_and_date(char *OutField)
{
  struct tm *dat;
  time_t timer;

  timer = time((time_t *)0);
  dat = localtime(&timer);
  dat->tm_mon += 1;
  dat->tm_year += 1900;
  sprintf(OutField, "%2.2d:%2.2d:%2.2d - %2.2d.%2.2d.%4.4d.", dat->tm_hour, dat->tm_min, dat->tm_sec, dat->tm_mday, dat->tm_mon, dat->tm_year);
  return (OutField);
}
char *
time_difference(char *OutField, char *InField1, char *InField2)
{
  long long prvi, drugi, razlika;

  prvi = timesecond(InField1);
  drugi = timesecond(InField2);
  if (prvi > drugi)
  {
    drugi *= (-1);
    prvi *= (-1);
  }
  razlika = (drugi - prvi);
  sprintf(OutField, "%lld", razlika);
  return (OutField);
}
long long
timesecond(char *InField)
{
  long long brs, brmn, brc, brd, brm, brg, ukupno;
  long long b, cl;
  char pomoc[64];
  char out[64];
  char *t;

  t = left(out, InField, 2);
  brc = atol(t);
  t = right(out, InField, 19);
  strcpy(pomoc, t);
  t = left(out, pomoc, 2);
  brmn = atol(t);
  t = right(out, InField, 16);
  strcpy(pomoc, t);
  t = left(out, pomoc, 2);
  brs = atol(t);
  t = right(out, InField, 11);
  strcpy(pomoc, t);
  t = left(out, pomoc, 2);
  brd = atol(t);
  t = right(out, InField, 9);
  strcpy(pomoc, t);
  t = left(out, pomoc, 2);
  brm = atol(t);
  t = right(out, InField, 5);
  strcpy(pomoc, t);
  t = left(out, pomoc, 2);
  brg = atol(t);
  brs += brc * 3600;
  brs += brmn * 60;
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

  brg -= 70;
  b = (long long)(brg / 4);
  cl = brg - ((long long)(brg / 4)) * 4;
  if (cl >= 2)
    b++;
  brg -= b;
  brg *= 525600;
  brg += (b * 527040);
  ukupno = brd + brm + brg;

  ukupno *= 60;
  ukupno += brs;
  return (ukupno);
}
char *
vrmuredi(char *OutField, char *InField)
{
  long long razlika, ostatak;
  char pom[50];

  razlika = atol(InField);
  ostatak = razlika / 3600;
  if (ostatak >= 1)
  {
    if (ostatak < 10)
    {
      strcpy(OutField, "0");
      sprintf(pom, "%1.0lld", ostatak);
      strcat(OutField, pom);
    }
    else
      sprintf(OutField, "%2.0lld", ostatak);
    strcat(OutField, ":");
    razlika = razlika - 3600 * ostatak;
    ostatak = razlika / 60;
    if (ostatak >= 1)
    {
      if (ostatak < 10)
      {
        strcat(OutField, "0");
        sprintf(pom, "%1.0lld", ostatak);
      }
      else
        sprintf(pom, "%2.0lld", ostatak);
      strcat(OutField, pom);
      strcat(OutField, ":");
      ostatak = razlika - 60 * ostatak;
      if (ostatak < 10)
      {
        strcat(OutField, "0");
        sprintf(pom, "%1.0lld", ostatak);
      }
      else
        sprintf(pom, "%2.0lld", ostatak);
      strcat(OutField, pom);
    }
    else
    {
      strcat(OutField, "00:");
      ostatak = razlika - 3600 * ostatak;
      if (ostatak < 10)
      {
        strcat(OutField, "0");
        sprintf(pom, "%1.0lld", ostatak);
      }
      else
        sprintf(pom, "%2.0lld", ostatak);
      strcat(OutField, pom);
    }
  }
  else
  {
    strcpy(OutField, "00:");
    ostatak = razlika / 60;
    if (ostatak >= 1)
    {
      if (ostatak < 10)
      {
        strcat(OutField, "0");
        sprintf(pom, "%1.0lld", ostatak);
      }
      else
        sprintf(pom, "%2.0lld", ostatak);
      strcat(OutField, pom);
      strcat(OutField, ":");
      ostatak = razlika - 60 * ostatak;
      if (ostatak < 10)
      {
        strcat(OutField, "0");
        sprintf(pom, "%1.0lld", ostatak);
      }
      else
        sprintf(pom, "%2.0lld", ostatak);
      strcat(OutField, pom);
    }
    else
    {
      strcat(OutField, "00:");
      ostatak = razlika;
      if (ostatak < 10)
      {
        strcat(OutField, "0");
        sprintf(pom, "%1.0lld", ostatak);
      }
      else
        sprintf(pom, "%2.0lld", ostatak);
      strcat(OutField, pom);
    }
  }
  return (OutField);
}
