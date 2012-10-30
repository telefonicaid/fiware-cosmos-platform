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
#ifndef PA_ENV_VALS_H
#define PA_ENV_VALS_H

/* ****************************************************************************
 *
 * FILE                  paEnvVals.h
 *
 * AUTHOR                Ken Zangelin
 *
 */
#include "parseArgs/parseArgs.h"          /* PaArgument                                */



/* ****************************************************************************
 *
 * paEnvName - get real name of variable (environment or RC-file variable)
 */
extern char *paEnvName(PaiArgument *aP, char *out);



/* ****************************************************************************
 *
 * paEnvVals -
 */
extern int paEnvVals(PaiArgument *paList);

#endif
