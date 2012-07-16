#ifndef SAMSON_LOG_MSG_H
#define SAMSON_LOG_MSG_H

#include <stdio.h>                /* printf, ...                             */
#include <string.h>               /* strerror                                */
#include <errno.h>                /* errno                                   */



/* ****************************************************************************
*
* LOG_ERROR - 
*/
#define LOG_ERROR(s)                                                          \
do                                                                            \
{                                                                             \
	printf("%s[%d]:%s: ", __FILE__, __LINE__, __FUNCTION__);                  \
	printf s;                                                                 \
	printf("\n");                                                             \
} while (0)

#endif
