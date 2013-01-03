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
#ifndef SAMSON_TYPES_H
#define SAMSON_TYPES_H

/* ****************************************************************************
 *
 * FILE               samsonTypes.h - basic data types used in SAMSON
 *
 *
 *
 *
 */
#include <sys/types.h>           /* size_t                                   */



/* ****************************************************************************
 *
 * Basic types
 */
typedef size_t          ss_uint64;
typedef unsigned int    ss_uint32;
typedef unsigned char   ss_uint8;
typedef int             ss_int32;
typedef char            ss_int8;
typedef float           ss_float;
typedef double          ss_double;
typedef size_t          ss_uint;                        // Variable length unsigned integuer

#endif
