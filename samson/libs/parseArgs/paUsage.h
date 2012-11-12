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
#ifndef PA_USAGE_H
#define PA_USAGE_H

/* ****************************************************************************
*
* FILE                  paUsage.h
*
* AUTHOR                Ken Zangelin
*
*/
#include <stdio.h>                        /* FILE                            */

#include "parseArgs/parseArgs.h"          /* PaArgument                      */



/* ****************************************************************************
*
* paUsage - print synopsis
*/
void paUsage(void);



/* ****************************************************************************
*
* paVersionPrint - print synopsis
*/
void paVersionPrint(void);



/* ****************************************************************************
*
* paExtendedUsage - print extended synopsis
*/
void paExtendedUsage(void);



/* ****************************************************************************
*
* paHelp - print help text
*/
void paHelp(void);

#endif
