/* EXAMPLE1.H */

#ifndef _EXAMPLE_1_H
#define _EXAMPLE_1_H

struct Record {
    long long Deleted;
    char client_id[65];
    char group_id[65];
    char resource_id[65];
    char auth_id[65];
    char socket_id[48];
    char status[3];
    char timestamp_l[11];
    char timestamp_h[11];
};

extern struct Record *info;
extern struct Record MyRecord;
extern struct DataExt DExt;
extern DataFilePtr DPtr;

extern struct IndexExt IExt_client_id;
extern DataFilePtr IPtr_client_id;
extern struct IndexExt IExt_group_id;
extern DataFilePtr IPtr_group_id;
extern struct IndexExt IExt_resource_id;
extern DataFilePtr IPtr_resource_id;
extern struct IndexExt IExt_auth_id;
extern DataFilePtr IPtr_auth_id;
extern struct IndexExt IExt_status;
extern DataFilePtr IPtr_status;

extern long long TaRecNum;

Boolean check();
void sig_handler(int sig);
void uuidt(char *uid);
void hex_string(char *str, int length);
void prepare_str(char *str, int length);
void empty_str(char *str, int length);
void trim_str(char *str);
void left_str(char *str, int length);


#endif  /* _EXAMPLE_1_H */
