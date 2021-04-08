/**************************************************************************/
/* PROGRAM: CHECK1.C                                                      */
/* DESCRIPTION: Check program for example functions.                      */
/* AUTHOR: VLADAN DJORDJEVIC                                              */
/**************************************************************************/
#include "screen.h"
#include "maccess.h"
#include "example1.h"

/**************************************************************************/
Boolean
check (input)
char *input;
{
  int _choice, sl, i, j;
  AchMatPtr Mat;
  char temp[65], remember[67];
  static int LastChoice;

  strcpy (temp, input);
  prepare_str (temp, 64);
  strcpy (remember, temp);
  trim_str (remember);
  sl = strlen (remember);
  SearchKey (IPtr_client_id, &IExt_client_id, &TaRecNum, (TaKeyStr *) temp);
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
      left_str (temp, sl);
      if (strcmp (remember, temp) == 0)
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
              NextKey (IPtr_client_id, &IExt_client_id, &TaRecNum, (TaKeyStr *) temp);
              left_str (temp, sl);
              if (OKAY == T && strcmp (remember, temp) == 0)
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
  _choice = Achoice ("CHOOSE:", Mat, LINES - 15, 64, 15, 2, i, &LastChoice, T);
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
      left_str (Mat[_choice - 1], 64);
      strcpy (input, Mat[_choice - 1]);
      mvprintw (6, 21, "KEY:");
      mvprintw (7, 21, "%s", input);
      refresh ();
      for (j = i, i = 0; i < j; i++)
        free (Mat[i]);
      return (T);
    }
}
/**************************************************************************/
