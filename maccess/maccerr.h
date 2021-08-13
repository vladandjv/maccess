/**************************************************************************/
/* MACCERR.H                                                              */
/* DESCRIPTION: The errors definitions.                                   */
/* AUTHOR: VLADAN DJORDJEVIC                                              */
/**************************************************************************/

#ifndef _MACCERR_H
#define _MACCERR_H

#define RecTooLarge 0
#define RecTooSmall 1
#define KeyTooLarge 2
#define RecSizeMismatch 3
#define KeySizeMismatch 4
#define ErrMakeFile 5
#define ErrReadHeader 6
#define ErrOpenFile 7
#define ErrCloseFile 8
#define ErrFlushFile 9
#define ErrMakeIndex 10
#define ErrOpenIndex 11
#define ErrReturnPage 12
#define ErrGetRec 13
#define ErrPutRec 14
#define ErrWriteRec 15
#define ErrLock1 16
#define ErrLock2 17
#define ErrUnLock1 18
#define ErrUnLock2 19
#define ErrGetSM 20
#define ErrAtSM 21
#define ErrDtSM 22
#define ErrRmSM 23
#define ErrMemoryCode 24
#define ErrSemaphoreSet 25
#define ErrSemaphoreAttach 26

extern long long IOStatus; /* word type */

#endif /* _MACCERR_H */
