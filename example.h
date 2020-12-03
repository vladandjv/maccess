/* EXAMPLE.H */

#ifndef _EXAMPLE_H
#define _EXAMPLE_H

struct Record {
    long long Deleted;
    char Key[11];
    char Surname[26];
    char Name[21];
    char Remark[46];
};

extern struct Record *info;
extern struct Record OurRecord;
extern struct DataExt DExt;
extern struct IndexExt IExt;
extern DataFilePtr DPtr;
extern DataFilePtr IPtr;
extern long long TaRecNum;

Boolean check();
void sig_handler(int sig);

#endif  /* _EXAMPLE_H */
