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
#ifndef PA_FULL_NAME_H
#define PA_FULL_NAME_H

/* ****************************************************************************
 *
 * FILE                  paFullName.h
 *
 * AUTHOR                Ken Zangelin
 *
 */
#include "parseArgs/parseArgs.h"          /* PaArgument                                */



/* ****************************************************************************
 *
 * paFullName - fill in name and description in string string
 */
extern char *paFullName(char *string, PaiArgument *aP);

#endif
