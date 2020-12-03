/* EXAMPLE2.H */

#ifndef _EXAMPLE_2_H
#define _EXAMPLE_2_H

struct Record_origin {
    long long Deleted;
    char record_id[65];
    char origin_name[65];
    int allowed;
};

struct Record_origin R_origin;
struct DataExt DExt_origin;
DataFilePtr DPtr_origin;

struct IndexExt IExt_origin_record_id;
DataFilePtr IPtr_origin_record_id;
struct IndexExt IExt_origin_origin_name;
DataFilePtr IPtr_origin_origin_name;

struct Record_ip_address {
    long long Deleted;
    char record_id[65];
    char ip4_add[17];
    char allowed[2]; /* char, not int, bacause it is indexed */
};

struct Record_ip_address R_ip_address;
struct DataExt DExt_ip_address;
DataFilePtr DPtr_ip_address;

struct IndexExt IExt_ip_address_record_id;
DataFilePtr IPtr_ip_address_record_id;
struct IndexExt IExt_ip_address_ip4_add;
DataFilePtr IPtr_ip_address_ip4_add;
struct IndexExt IExt_ip_address_allowed;
DataFilePtr IPtr_ip_address_allowed;

struct Record_auth_id_allowed {
    long long Deleted;
    char record_id[65];
    char auth_id[65];
};

struct Record_auth_id_allowed R_auth_id_allowed;
struct DataExt DExt_auth_id_allowed;
DataFilePtr DPtr_auth_id_allowed;

struct IndexExt IExt_auth_id_allowed_record_id;
DataFilePtr IPtr_auth_id_allowed_record_id;
struct IndexExt IExt_auth_id_allowed_auth_id;
DataFilePtr IPtr_auth_id_allowed_auth_id;

struct Record_resource {
    long long Deleted;
    char record_id[65];
    char resource_id[33];
    char bash_script[256];
    char special_use[64];
};

struct Record_resource R_resource;
struct DataExt DExt_resource;
DataFilePtr DPtr_resource;

struct IndexExt IExt_resource_record_id;
DataFilePtr IPtr_resource_record_id;
struct IndexExt IExt_resource_resource_id;
DataFilePtr IPtr_resource_resource_id;

long long TaRecNum;

void sig_handler(int sig);
void uuidt(char *uid);
void hex_string(char *str, int length);
void prepare_str(char *str, int length);
void empty_str(char *str, int length);
void trim_str(char *str);
void left_str(char *str, int length);

#endif  /* _EXAMPLE_2_H */
