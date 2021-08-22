/**************************************************************************/
/* MACCESS.H                                                              */
/* Definitions for the maccess functions                                  */
/* AUTHOR: VLADAN DJORDJEVIC                                              */
/**************************************************************************/

#ifndef _MACCESS_H
#define _MACCESS_H

#include "defs.h"
#include "maccerr.h"
/************************** constants *************************************/
#define MaxKeyLen (unsigned long long)35
#define PageSize (long long)1016
#define PageStackSize (long long)1020
#define Order (long long)(PageSize / (long long)2)
#define MaxHeight (long long)5 /* run ./btree_parameters for the calculation */
#define FileHeaderSize (sizeof(struct FileHeader))
#define MinDataRecSize FileHeaderSize
#define ItemOverhead (sizeof(struct TaItem) - MaxKeyLen)
/* For PageOverhead see defined bellow, line 48 */

/*Theory: (but it is not rational to be more then needed)
#define MaxDataRecSize (PageOverhead + PageSize * (ItemOverhead + MaxKeyLen)) */
#define MaxDataRecSize (long long)150

#define NoDuplicates (long long)0
#define Duplicates (long long)1
/****************************** strustures and types **********************/
typedef char FileName[MAX_FILENAME_LENGHT];
typedef char TaKeyStr[MaxKeyLen];

enum Bool
{
    F = (long long)0,
    T = (long long)1
};
typedef enum Bool Boolean;

struct TaItem
{
    long long DataRef;
    long long PageRef;
    TaKeyStr Key;
};
typedef struct TaItem ArrItem[PageSize];

struct TaPage
{
    long long ItemsOnPage;
    long long BckwPageRef;
    ArrItem ItemArray;
};
typedef struct TaPage *TaPagePtr;

#define PageOverhead (sizeof(((struct TaPage *)0)->ItemsOnPage) + sizeof(((struct TaPage *)0)->BckwPageRef))

struct FileHeader
{
    size_t FirstFree;
    long long NumberFree;
    long long Int1;
    size_t ItemSize;
    size_t NumRec;
};

struct DataFile
{
    struct FileHeader Header;
    long long FileNumber;
    long long RR;
    unsigned long long KeyL; /* byte type */
};
typedef struct DataFile *DataFilePtr;

struct DataExt
{
    long long FileNumber;
    FileName FileTitle;
    long long C;
    long long H;
};

struct TaSearchStep
{
    long long PageRef;
    long long ItemArrIndex;
    long long HasBits;
};
typedef struct TaSearchStep TaPath[MaxHeight];

struct IndexExt
{
    struct DataExt DataE;
    Boolean AllowDuplKeys;
    long long PP;
    TaPath Path;
    long long LastRecord;
    TaKeyStr LastKey;
};
typedef struct IndexExt *IndexExtPtr;

struct TaStackRec
{
    struct TaPage Page;
    DataFilePtr IndexFPtr;
    IndexExtPtr IndexEPtr;
    long long PageRef;
    Boolean Updated;
};
typedef struct TaStackRec *TaStackRecPtr;
typedef struct TaStackRec TaPageStack[PageStackSize];
typedef TaPageStack *TaPageStackPtr;

union TaRecordBuffer
{
    struct TaStackRec Page;
    char R[MaxDataRecSize]; /* byte type */
    long long I;
};

typedef long long TaPageMap[PageStackSize];
typedef TaPageMap *TaPageMapPtr;
/***************************** variables  ********************************/
extern union TaRecordBuffer Buffer;
extern union TaRecordBuffer *TaRecBuf;
extern TaPageStack *TaPageStk;
extern TaPageMap *TaPgMap;
extern char *MemSh;
extern Boolean OKAY;
extern long long MemId;

#define MACCESS_SHM_MEM_SIZE (size_t)((PageSize * sizeof(struct DataFile)) + sizeof(TaPageStack) + sizeof(TaPageMap))
/***************************** functions **********************************/
#if __cplusplus
extern "C"
{
#endif /* __cplusplus */

    void TaGetRec(), TaPutRec(), TAIOCheck(), TaReturnPage(), LeaveSM();
    long long StartSM(), ApproachSM(), FindYourPlace();
    char *USM();

    void GetRec(), PutRec(), MakeFile(), MakeIndex();
    void OpenFile(), OpenIndex(), CloseFile(), CloseIndex();
    void FlushFile(), FlushIndex(), AddRec(), AddKey();
    void DeleteRec(), DeleteKey(), SearchKey(), FindKey();
    void NextKey(), PrevKey(), ClearKey();

    long long FileLen(), UsedRecs();

    /* Shared memory functions */
    void InitAccess(), TermAccess();
    long long Destroy_SHM();

    /* Locking system functions */
    void DB_Lock_Init(key_t semaphore_code);
    void SHM_Lock(), SHM_UnLock();
    long long Destroy_Semaphore();

#if __cplusplus
};
#endif /* __cplusplus */

#endif /* _MACCESS_H */
       /******************************* END **************************************/
