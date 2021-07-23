/**************************************************************************
 *      STRINGV.H
 *	Definitions for the string rutines.
 *	Author: Vladan Djordjevic
 **************************************************************************/
#ifndef _STRINGV_H
#define _STRINGV_H

#include <string.h>

#if __cplusplus
extern "C"
{
#endif /* __cplusplus */
	char *vsubstr(char *OutField, char *InField, long long start, long long size);
	char *left(char *OutField, char *InField, long long size);
	char *right(char *OutField, char *InField, long long size);
	char *ltrim(char *OutField, char *InField);
	char *trim(char *OutField, char *InField);
	char *to_right(char *OutField, char *InField, long long size);
	char *to_left(char *OutField, char *InField, long long size);
	char *empty_string(char *OutField, long long size);
	char *repeat(char *OutField, char Symbol, long long size);
	long long isEmpty(char *InField);
#if __cplusplus
};
#endif /* __cplusplus */

#endif /* _STRINGV_H */
       /******************************* END **************************************/
