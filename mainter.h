/**************************************************************************
 *      MAINTER.H
 *	Definitions for maccess.c rutines.
 *	All Rights Reserved.
 **************************************************************************/
#ifndef _MAINTER_H
#define _MAINTER_H

#include "maccess.h"
#include "maccerr.h"

/****************************** types *************************************/
struct TaItem {
    long long DataRef;
    long long PageRef;
    TaKeyStr Key;
};

typedef struct TaItem ArrItem[PageSize];

struct TaPage {
    long long ItemsOnPage;
    long long BckwPageRef;
    ArrItem ItemArray;
};

typedef struct TaPage *TaPagePtr;

struct TaStackRec {
    struct TaPage Page;
    DataFilePtr IndexFPtr;
    IndexExtPtr IndexEPtr;
    long long PageRef;
    Boolean Updated;
};

typedef struct TaStackRec *TaStackRecPtr;

typedef struct TaStackRec TaPageStack[PageStackSize];

typedef TaPageStack *TaPageStackPtr;

typedef long long TaPageMap[PageStackSize];

typedef TaPageMap *TaPageMapPtr;

union TaRecordBuffer {
    struct TaStackRec Page;
    char R[MaxDataRecSize]; /* byte type */
    long long I;
};
/***************************** variables  ********************************/
extern union TaRecordBuffer Buffer;
extern union TaRecordBuffer *TaRecBuf;
extern TaPageStack *TaPageStk[HasMemories];
extern TaPageMap *TaPgMap[HasMemories];

extern char *MemSh[HasMemories];
/***************************** functions **********************************/

#if __cplusplus
extern "C" {
#endif /* __cplusplus */

    void TaGetRec(), TaPutRec(), TAIOCheck(), TaReturnPage(), LeaveSM();
    long long StartSM(), ApproachSM(), FindYourPlace();
    char *USM();

#if __cplusplus
};
#endif /* __cplusplus */

#endif  /* _MAINTER_H */
/******************************* END **************************************/
