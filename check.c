/**************************************************************************/
/* PROGRAM: CHECK.C                                                       */
/* DESCRIPTION: Check program for example functions.                      */
/* AUTHOR: VLADAN DJORDJEVIC                                              */
/**************************************************************************/
#include "screen.h"
#include "maccess.h"
#include "example.h"

DataFilePtr IPtr = NULL;
/**************************************************************************/
Boolean
check (input)
char *input;
{
  int _choice, sl, i, j;
  AchMatPtr Mat;
  char temp[11], remember[11];
  static int LastChoice;

  strcpy (temp, to_left (input, 10));
  strcpy (remember, temp);
  strcpy (remember, trim (remember));
  sl = strlen (remember);
  SearchKey (IPtr, &IExt, &TaRecNum, (TaKeyStr *) temp);
  if (OKAY == F)
    {
      if (isEmpty (input) == F)
        {
          message ("There is no requested root of key!");
          return (F);
        }
      else
        {
          message ("There is no any key entered in the base.");
          return (F);
        }
    }
  else
    {
      if (strcmp (temp, remember) == 0)
        return (T);
      if (strcmp (remember, left (temp, sl)) == 0)
        {
          if ((Mat[0] = (char *) malloc (strlen (temp) + 1)) == NULL)
            {
              message ("Not enough memory!");
              return (F);
            }
          strcpy (Mat[0], temp);
          strcat (Mat[0], " ");
          for (i = 1; i < MasAchRow; i++)
            {
              NextKey (IPtr, &IExt, &TaRecNum, (TaKeyStr *) temp);
              if (OKAY == T && strcmp (remember, left (temp, sl)) == 0)
                {
                  if ((Mat[i] = (char *) malloc (strlen (temp) + 1)) == NULL)
                    {
                      for (j = i, i = 0; i < j; i++)
                        free (Mat[i]);
                      message ("List too long! Enter some character to short it!");
                      return (F);
                    }
                  strcpy (Mat[i], temp);
                  strcat (Mat[i], " ");
                }
              else
                break;
            }
          if (i == MasAchRow)
            {
              for (j = i, i = 0; i < j; i++)
                free (Mat[i]);
              message ("List too long! Enter some character to short it!");
              return (F);
            }
        }
      else
        {
          message ("There is no requested root of key!");
          return (F);
        }
    }
  savescreen ();
  _choice = Achoice ("CHOOSE:", Mat, LINES - 11, 11, 5, 65, i, &LastChoice);
  restscreen ();
  switch (_choice)
    {
    case -1:
      for (j = i, i = 0; i < j; i++)
        free (Mat[i]);
      return (F);
    case -2:
      Direct_Exit = T;
      for (j = i, i = 0; i < j; i++)
        free (Mat[i]);
      return (F);
    default:
      strcpy (input, left (Mat[_choice - 1], 10));
      mvprintw (6, 21, "KEY:     %s", input);
      refresh ();
      for (j = i, i = 0; i < j; i++)
        free (Mat[i]);
      return (T);
    }
}
/**************************************************************************/
