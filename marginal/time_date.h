/**************************************************************************
 *      TIME_DATE.H
 *	Definitions for the time and rutines.
 *	Author: Vladan Djordjevic
 **************************************************************************/
#ifndef _TIME_DATE_H
#define _TIME_DATE_H

#include <time.h>

#if __cplusplus
extern "C"
{
#endif /* __cplusplus */

	char *my_date(), *datplus(char *OutField, char *InField, long long dani);
	char *datrazl(char *OutField, char *InField1, char *InField2);
	char *okrenidat(char *OutField, char *InField);
	char *my_time(), *time_and_date();
	char *time_difference(char *OutField, char *InField1, char *InField2);
	char *vrmuredi(char *OutField, char *InField);

#if __cplusplus
};
#endif /* __cplusplus */

#endif /* _TIME_DATE_H */
/******************************* END **************************************/
