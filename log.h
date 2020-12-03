/* log.h */

#ifndef _H_LOG
#define _H_LOG

#include "conf_lib.h"

#include <stdio.h>

#if __cplusplus
extern "C" {
#endif /* __cplusplus */

    void openLog(char *path);
    void logMessage(char * s, ...);
    void closeLog();
    void setLogfile(char *path);

#if __cplusplus
};
#endif /* __cplusplus */

#endif /* _H_LOG */
