/**************************************************************************
 *      MACCESS.H
 *	Definitions for maccess.c rutines.
 **************************************************************************/
#ifndef _MACCESS_H
#define _MACCESS_H

#include "defs.h"
/************************** constants *************************************/
/*If MaxKeyLen change, you also have to change IteOverHead and shared
 * memory segment size (in conf_lib.h). It is the best to run 
 * ./btree_parameters for calculation */
#define MaxKeyLen  11 /* Read the above! */
#define PageSize   254
#define PageStackSize 255
#define Order 127
#define MaxHeight 6 
#define FileHeaderSize 32 /* (sizeof(long long)*3 + sizeof(long long))  */
#define MinDataRecSize  FileHeaderSize
#define ItemOverhead 16 /* sizeof(long long) * 2 */
#define PageOverhead 16 /* (sizeof(long long)+sizeof(long long)) */
/*Theory: MaxDataRecSize=(PageOverhead + PageSize * (ItemOverhead + MaxKeyLen)) */
// #define MaxDataRecSize PageOverhead + PageSize * (ItemOverhead + MaxKeyLen) 
#define MaxDataRecSize 512

#define NoDuplicates      (long long)0
#define Duplicates        (long long)1
#define HasMemories       30 /* Maximum number of shared memory segments */
/****************************** types *************************************/
typedef char FileName[MAX_STRING_LENGTH];

struct HederFajla {
    long long FirstFree;
    long long NumberFree;
    long long Int1;
    long long ItemSize;
    long long NumRec;
};

struct DataFile {
    long long M;
    struct HederFajla Heder;
    long long FileNumber;
    long long RR;
    unsigned long long KeyL; /* byte type */
};

typedef struct DataFile *DataFilePtr;

struct DataExt {
    long long FileNumber;
    FileName FileTitle;
    long long C;
    long long H;
};

typedef char TaKeyStr[MaxKeyLen];

struct TaSearchStep {
    long long PageRef;
    long long ItemArrIndex;
    long long HasBits;
};

typedef struct TaSearchStep TaPath[MaxHeight];

struct IndexExt {
    struct DataExt DataE;
    Boolean AllowDuplKeys;
    long long PP;
    TaPath Path;
    long long LastRecord;
    TaKeyStr LastKey;
};

typedef struct IndexExt *IndexExtPtr;
/***************************** variables  ********************************/
extern Boolean OKAY;
extern long long MemId[HasMemories];
/***************************** functions **********************************/

#if __cplusplus
extern "C" {
#endif /* __cplusplus */

    void GetRec(), PutRec(), MakeFile(), MakeIndex();
    void OpenFile(), OpenIndex(), CloseFile(), CloseIndex();
    void FlushFile(), FlushIndex(), AddRec(), AddKey();
    void DeleteRec(), DeleteKey(), SearchKey(), FindKey();
    void NextKey(), PrevKey(), ClearKey(), InitAccess();
    void TermAccess(), EraSM();

    long long FileLen(), UsedRecs();

#if __cplusplus
};
#endif /* __cplusplus */

#endif  /* _MACCESS_H */
/******************************* END **************************************/
