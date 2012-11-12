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
#ifndef PA_OPTIONS_PARSE_H
#define PA_OPTIONS_PARSE_H

/* ****************************************************************************
*
* FILE                  paOptionsParse.c - 
*
* AUTHOR                Ken Zangelin
*
*/
#include "parseArgs/parseArgs.h"          /* PaArgument                                */



/* ****************************************************************************
*
* paOptionsParse - 
*/
extern int paOptionsParse(PaiArgument* paList, char* argV[], int argC);

#endif
