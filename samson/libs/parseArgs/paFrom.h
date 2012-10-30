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
#ifndef PA_FROM_H
#define PA_FROM_H

/* ****************************************************************************
 *
 * FILE                  paFrom.h
 *
 * AUTHOR                Ken Zangelin
 *
 */
#include "baStd.h"               /* BA standard header file                   */

#include "parseArgs/parseArgs.h"  /* PaArgument                                */



/* ****************************************************************************
 *
 * paFromName -
 */
extern char *paFromName(PaiArgument *aP, char *out);



/* ****************************************************************************
 *
 * paFrom - from where did the value come?
 */
extern char *paFrom(PaiArgument *paList, const char *option);

#endif
