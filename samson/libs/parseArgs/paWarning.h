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
#ifndef PA_WARNINGS_H
#define PA_WARNINGS_H

/* ****************************************************************************
 *
 * FILE                  paWarning.h
 *
 * AUTHOR                Ken Zangelin
 *
 */
#include "logMsg/logMsg.h"             /* LM_W                                      */
#include "parseArgs/parseArgs.h"       /* PaSeverity, PaWarning                     */



/* ****************************************************************************
 *
 * PA_WARNING
 */
#define PA_WARNING(s, txt) \
  do                         \
  {                          \
    paWarningAdd(s, txt);  \
  } while (0)



/* ****************************************************************************
 *
 * paWarningInit -
 */
extern void paWarningInit(void);



/* ****************************************************************************
 *
 * paWarningAdd -
 */
extern void paWarningAdd(PaSeverity severity, char *txt);

#endif  // ifndef PA_WARNINGS_H
