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
#ifndef PA_LIMIT_CHECK_H
#define PA_LIMIT_CHECK_H

/* ****************************************************************************
 *
 * FILE                  paLimitCheck.h -  check option values are within limits
 *
 * AUTHOR                Ken Zangelin
 *
 */
#include "parseArgs/parseArgs.h"          /* Own interface                             */



/* ****************************************************************************
 *
 * paLimitCheck - check limits for all options
 */
extern int paLimitCheck(PaiArgument *paList);

#endif

