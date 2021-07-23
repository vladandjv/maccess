/**************************************************************************
 *  STRING.C
 *  Routines for string manipulations 
 *	Author: Vladan Djordjevic
 **************************************************************************/

#include <stdlib.h>
#include "stringv.h"

/* The vsubstr function copies the string pointed to by InField to the buffer 
 * pointed to by OutField, starting from the character number defined by the
 * start variable till the 'start + size' character as the last one. The 
 * terminating null byte ('\0') is included. 
 */
char *vsubstr(char *OutField, char *InField, long long start, long long size)
{
  long long i, len;

  len = strlen(InField);
  if (len <= start)
    return InField;
  if (len <= start + size)
  {
    strcpy(OutField, InField + start);
    return (OutField);
  }
  for (i = 0; i < size; i++)
    OutField[i] = *(InField + start + i);
  OutField[size] = '\0';
  return OutField;
}
/* The left function copies the string pointed to by InField to the buffer 
 * pointed to by OutField, but cut for the lenght defined by size from the
 * left side. The terminating null byte ('\0') is included. 
 */
char *
left(char *OutField, char *InField, long long size)
{
  if (strlen(InField) <= size)
  {
    strcpy(OutField, InField);
    return (OutField);
  }
  strncpy(OutField, InField, size);
  OutField[size] = '\0';
  return (OutField);
}
/* The right function copies the string pointed to by InField to the buffer 
 * pointed to by OutField, but cut for the lenght defined by size from the
 * right side. The terminating null byte ('\0') is included. 
 */
char *
right(char *OutField, char *InField, long long size)
{
  long long i, start;

  if (strlen(InField) <= size)
  {
    strcpy(OutField, InField);
    return (OutField);
  }
  start = strlen(InField);
  for (i = size; i > 0; i--)
    OutField[size - i] = *(InField + start - i);
  OutField[size] = '\0';
  return (OutField);
}
/* The ltrim function copies the string pointed to by InField to the buffer 
 * pointed to by OutField, but without leading blanks. The terminating null 
 * byte ('\0') is included. 
 */
char *
ltrim(char *OutField, char *InField)
{
  long long i, len;

  len = strlen(InField);
  for (i = 0; *(InField + i) != '\0'; i++)
  {
    if (*(InField + i) != ' ')
      return (vsubstr(OutField, InField, i, len));
  }
  OutField[0] = '\0';
  return (OutField);
}
/* The trim function copies the string pointed to by InField to the buffer 
 * pointed to by OutField, but without ending blanks. The terminating null 
 * byte ('\0') is included. 
 */
char *
trim(char *OutField, char *InField)
{
  long long i, start;

  start = strlen(InField);
  strcpy(OutField, InField);
  for (i = start - 1; i >= 0; i--)
  {
    if (OutField[i] != ' ')
    {
      OutField[i + 1] = '\0';
      return (OutField);
    }
  }
  OutField[0] = '\0';
  return (OutField);
}
/* The to_right function copies the string pointed to by InField to the buffer 
 * pointed to by OutField. If the string InField has lenght less then it is 
 * required by size then the space is filled with ending blanks, otherwise it is cut.
 * The terminating null byte ('\0') is included. 
 */
char *
to_right(char *OutField, char *InField, long long size)
{
  char *intermediate, *k;

  trim(OutField, InField);
  k = (char *)malloc(sizeof(char) * strlen(OutField));
  ltrim(k, OutField);
  empty_string(OutField, size);
  intermediate = (char *)malloc(sizeof(char) * (strlen(k) + size) + 1);
  strcpy(intermediate, OutField);
  strcat(intermediate, k);
  free(k);
  right(OutField, intermediate, size);
  free(intermediate);
  return (OutField);
}
/* The to_left function copies the string pointed to by InField to the buffer 
 * pointed to by OutField. If the string InField has lenght less then it is 
 * required by size then the space is filled with leading blanks, otherwise it is cut.
 * The terminating null byte ('\0') is included. 
 */
char *
to_left(char *OutField, char *InField, long long size)
{
  char *intermediate, *k;

  trim(OutField, InField);
  k = (char *)malloc(sizeof(char) * strlen(OutField));
  ltrim(k, OutField);
  empty_string(OutField, size);
  intermediate = (char *)malloc(sizeof(char) * (strlen(k) + size) + 1);
  strcpy(intermediate, k);
  strcat(intermediate, OutField);
  free(k);
  left(OutField, intermediate, size);
  free(intermediate);
  return (OutField);
}
/* The empty_string function copies 'size' number of blanks to the buffer 
 * by OutField. The terminating null byte ('\0') is included. 
 */
char *
empty_string(char *OutField, long long size)
{
  long long i;

  for (i = 0; i < size; i++)
    OutField[i] = ' ';
  OutField[size] = '\0';
  return (OutField);
}
/* The repeat function copies 'size' number of characters defined by the Symbol 
 * variable to the pointed to by OutField. The terminating null byte ('\0') 
 * is included. 
 */
char *
repeat(char *OutField, char Symbol, long long size)
{
  long long i;

  for (i = 0; i < size; i++)
    OutField[i] = Symbol;
  OutField[size] = '\0';
  return (OutField);
}
/* The isEmpty function checks if the string InField it is empty (consists only
 * of blanks) and returns 0 if it is, otherwise returns 1. 
 */
long long
isEmpty(char *InField)
{
  long long i;

  for (i = 0; *(InField + i) != '\0'; i++)
  {
    if (*(InField + i) != ' ')
      return 0;
  }
  return 1;
}
