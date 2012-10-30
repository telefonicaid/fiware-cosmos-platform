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
#ifndef PA_ITERATE_H
#define PA_ITERATE_H

/* ****************************************************************************
 *
 * FILE                  paIterate.h
 *
 * AUTHOR                Ken Zangelin
 *
 */
#include "parseArgs/parseArgs.h"          /* PaArgument                                */



/* ****************************************************************************
 *
 * paIterateInit -
 */
extern void paIterateInit(void);



/* ****************************************************************************
 *
 * paIterateNext -
 */
extern PaiArgument *paIterateNext(PaiArgument *paList);



/* ****************************************************************************
 *
 * paNoOfOptions -
 */
extern int paNoOfOptions(PaArgument *paList);



/* ****************************************************************************
 *
 * paOptionLookupByIx -
 */
extern PaArgument *paOptionLookupByIx(PaArgument *paList, int ix);

#endif  // ifndef PA_ITERATE_H
