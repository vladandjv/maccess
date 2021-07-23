/**************************************************************************/
/* LOG.C                                                                  */
/* DESCRIPTION: Writes in a log file                                      */
/* AUTHOR: VLADAN DJORDJEVIC                                              */
/**************************************************************************/

#include "defs.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>

static FILE *logfp = NULL;
static char *Logfile = NULL;

void openLog(char *path)
{
  if ((logfp = fopen(path, "a")) == NULL)
  {
    logfp = stderr;
    logMessage("Couldn't open %s, using STDERR!\n", path);
    return;
  }

  Logfile = (char *)malloc((strlen(path) + 1) * sizeof(char));
  strcpy(Logfile, path);
}

void closeLog()
{
  if (Logfile)
  {
    free(Logfile);
    Logfile = NULL;
  }
  if (logfp != NULL)
    fclose(logfp);
}

void logMessage(char *s, ...)
{
  va_list args;

  if (!logfp)
    return;

  va_start(args, s);
  fprintf(logfp, "* ");
  vfprintf(logfp, s, args);
  fprintf(logfp, "\n");
  va_end(args);

  fflush(logfp);
}

void setlogfile(char *path)
{
  if (path && ((!Logfile) || strcmp(path, Logfile)))
  {
    if (Logfile)
    {
      free(Logfile);
      Logfile = NULL;
    }
    if (logfp != NULL)
      fclose(logfp);

    if ((logfp = fopen(path, "a")) == NULL)
    {
      logfp = stderr;
      logMessage("Couldn't open %s, using STDERR!\n", path);
      return;
    }

    Logfile = (char *)malloc((strlen(path) + 1) * sizeof(char));
    strcpy(Logfile, path);
    setvbuf(logfp, NULL, _IONBF, 0);
  }
}
