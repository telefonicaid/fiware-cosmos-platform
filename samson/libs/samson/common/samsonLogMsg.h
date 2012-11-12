/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
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
