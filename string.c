/* STRING.C */
/* These functions should be used only with some gaOrdNumage colletor */

#include "defs.h"
char *
vsubstr (char *InField, int Poc, int Vel)
{
  char *OutField;
  int i, len;

  len = strlen (InField);
  OutField = (char *) malloc (sizeof (char) * Vel + 1);
  if (OutField == NULL)
    return NULL;
  if (len <= Poc)
    {
      free (OutField);
      return NULL;
    }
  if (len <= Poc + Vel)
    {
      strcpy (OutField, InField + Poc);
      return (OutField);
    }
  for (i = 0; i < Vel; i++)
    OutField[i] = *(InField + Poc + i);
  OutField[Vel] = '\0';
  return (OutField);
}
char *
left (char *InField, int Vel)
{
  char *OutField;

  OutField = (char *) malloc (sizeof (char) * Vel + 1);
  if (OutField == NULL)
    return NULL;
  if (strlen (InField) <= Vel)
    {
      strcpy (OutField, InField);
      return (OutField);
    }
  strncpy (OutField, InField, Vel);
  OutField[Vel] = '\0';
  return (OutField);
}
char *
right (char *InField, int Vel)
{
  char *OutField;
  int i, Poc;

  OutField = (char *) malloc (sizeof (char) * Vel + 1);
  if (OutField == NULL)
    return NULL;
  if (strlen (InField) <= Vel)
    {
      strcpy (OutField, InField);
      return (OutField);
    }
  Poc = strlen (InField);
  for (i = Vel; i > 0; i--)
    OutField[Vel - i] = *(InField + Poc - i);
  OutField[Vel] = '\0';
  return (OutField);
}
char *
ltrim (char *InField)
{
  char *OutField;
  int i, len;

  len = strlen (InField);
  for (i = 0; *(InField + i) != '\0'; i++)
    {
      if (*(InField + i) != ' ')
        return (vsubstr (InField, i, len));
    }
  OutField = (char *) malloc (1);
  OutField[0] = '\0';
  return (OutField);
}
char *
trim (char *InField)
{
  char *OutField;
  int i, Poc;

  Poc = strlen (InField);
  OutField = (char *) malloc (sizeof (char) * Poc + 1);
  if (OutField == NULL)
    return NULL;
  strcpy (OutField, InField);
  for (i = Poc - 1; i >= 0; i--)
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
char *
to_right (char *InField, int Vel)
{
  char *OutField, *intermediate, *k;

  OutField = trim (InField);
  k = ltrim (OutField);
  if (k == NULL)
    {
      free (OutField);
      return NULL;
    }
  free (OutField);
  intermediate = (char *) malloc (sizeof (char) * (strlen (k) + Vel) + 1);
  if (intermediate == NULL)
    {
      free (k);
      return NULL;
    }
  OutField = empty_string (Vel);
  strcpy (intermediate, OutField);
  strcat (intermediate, k);
  free (OutField);
  free (k);
  OutField = right (intermediate, Vel);
  free (intermediate);
  return (OutField);
}
char *
to_left (char *InField, int Vel)
{
  char *OutField, *intermediate, *k;

  OutField = trim (InField);
  k = ltrim (OutField);
  if (k == NULL)
    {
      free (OutField);
      return NULL;
    }
  free (OutField);
  intermediate = (char *) malloc (sizeof (char) * (strlen (k) + Vel) + 1);
  if (intermediate == NULL)
    {
      free (k);
      return NULL;
    }
  OutField = empty_string (Vel);
  strcpy (intermediate, k);
  strcat (intermediate, OutField);
  free (OutField);
  free (k);
  OutField = left (intermediate, Vel);
  free (intermediate);
  return (OutField);
}
char *
empty_string (int Vel)
{
  int i;
  char *OutField;

  OutField = (char *) malloc (sizeof (char) * Vel + 1);
  if (OutField == NULL)
    return NULL;
  for (i = 0; i < Vel; i++)
    OutField[i] = ' ';
  OutField[Vel] = '\0';
  return (OutField);
}
char *
ponovi (char Symbol, int Vel)
{
  char *OutField;
  int i;

  OutField = (char *) malloc (sizeof (char) * Vel + 1);
  if (OutField == NULL)
    return NULL;
  for (i = 0; i < Vel; i++)
    OutField[i] = Symbol;
  OutField[Vel] = '\0';
  return (OutField);
}
int
isEmpty (char *InField)
{
  int i;

  for (i = 0; *(InField + i) != '\0'; i++)
    {
      if (*(InField + i) != ' ')
        return 0;
    }
  return 1;
}
