/* SCRCHECK.C */
#include "scrdefs.h"
/**************************************************************************/
Boolean
check_number_empty (field)

char *field;
{
  unsigned int i;
  char intermediate[56];

  strcpy (intermediate, ltrim (field));
  strcpy (intermediate, trim (intermediate));
  for (i = 0; i < strlen (intermediate); i++)
    if (!isdigit (intermediate[i]))
      {
        message ("You can enter only numbers!");
        return (F);
      }
  return (T);
}
/**************************************************************************/
Boolean
check_number (field)

char *field;
{
  unsigned int i;
  char intermediate[56];

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
        message ("You can enter only numbers!");
        return (F);
      }
  return (T);
}
/**************************************************************************/
Boolean
check_character (field)

char *field;
{
  if (isEmpty (field) == T)
    {
      message ("Mandatory field!");
      return (F);
    }
  return (T);
}
/**************************************************************************/
Boolean
check_dot_empty (unos)

char *unos;
{
  unsigned int i, vel;
  char intermediate[MAX_STRING_LENGTH];

  if (isEmpty (unos) == T)
    return (T);
  strcpy (intermediate, ltrim (unos));
  strcpy (intermediate, trim (intermediate));
  if (strcmp (intermediate, "KRAJ") == 0)
    return (T);
  vel = strlen (intermediate);
  if (intermediate[0] == '-')
    {
      vel--;
      strcpy (intermediate, right (intermediate, vel));
    }
  for (i = 0; i < vel; i++)
    if (!isdigit (intermediate[i]))
      {
        if (intermediate[i] == '.')
          {
            if (intermediate[i + 1] == '\0')
              {
                message ("You haven't entered decimals, only decimal point!");
                return (F);
              }
            if (intermediate[i + 2] != '\0')
              {
                if (intermediate[i + 3] != '\0')
                  {
                    message ("Number can have only two decimal digits!");
                    return (F);
                  }
              }
          }
        else
          {
            message ("You can enter only numbers!");
            return (F);
          }
      }
  return (T);
}
/**************************************************************************/
Boolean
check_dot (unos)

char *unos;
{
  unsigned int i, vel;
  char intermediate[MAX_STRING_LENGTH];

  if (isEmpty (unos) == T)
    {
      message ("Mandatory field!");
      return (F);
    }
  strcpy (intermediate, ltrim (unos));
  strcpy (intermediate, trim (intermediate));
  vel = strlen (intermediate);
  if (intermediate[0] == '-')
    {
      vel--;
      strcpy (intermediate, right (intermediate, vel));
    }
  for (i = 0; i < vel; i++)
    if (!isdigit (intermediate[i]))
      {
        if (intermediate[i] == '.')
          {
            if (intermediate[i + 1] == '\0')
              {
                message ("You haven't entered decimals, only decimal point!");
                return (F);
              }
            if (intermediate[i + 2] != '\0')
              {
                if (intermediate[i + 3] != '\0')
                  {
                    message ("Number can have only two decimal digits!");
                    return (F);
                  }
              }
          }
        else
          {
            message ("You can enter only numbers!");
            return (F);
          }
      }
  if (atof (intermediate) == 0)
    {
      message ("You have entered zero. Sensless!");
      return (F);
    }
  return (T);
}
/**************************************************************************/
