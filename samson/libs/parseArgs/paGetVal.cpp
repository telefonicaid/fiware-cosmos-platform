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
#include <stdio.h>               /* stderr, stdout, ...                       */
#include <stdlib.h>              /* strtol, atoi                              */

#include "baStd.h"               /* BA standard header file                   */
#include "logMsg/logMsg.h"       /* lmVerbose, lmDebug, ...                   */

#include "paGetVal.h"            /* Own interface                             */
#include "paWarning.h"           /* paWarningAdd                              */
#include "parseArgs/parseArgs.h"  /* progName                                  */



/* ****************************************************************************
 *
 * paGetVal - calculate the integer value of a string
 */
void *paGetVal(char *string, int *error) {
  long value;
  int type;
  char errorText[256];

  errorText[0] = 0;

  *error = PaOk;

  value = baStoi(string, &type, errorText);
  if (errorText[0] != 0) {
    PA_WARNING(PasBadValue, errorText);
    *error = type;
    return NULL;
  }

  return (void *)value;
}

