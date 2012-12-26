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
/* ****************************************************************************
 *
 * FILE			paWarning.c -
 *
 * AUTHOR		Ken Zangelin
 *
 * HISTORY
 * $Log:$
 *
 *
 */
#include <stdlib.h>               /* free, ...                                  */
#include <string.h>               /* strdup, ...                                */

#include "baStd.h"                /* BA_VEC_SIZE, ...                          */
#include "logMsg/logMsg.h"        /* lmTraceSet                                */

#include "paPrivate.h"            /* PaTypeUnion, config variables, ...        */
#include "paTraceLevels.h"        /* LmtPaDefaultValues, ...                   */
#include "paWarning.h"            /* Own interface                             */
#include "parseArgs/parseArgs.h"  /* paWarnings, paWarning                     */



/* ****************************************************************************
 *
 * paWarning -
 */
PaWarning paWarning[100];
int paWarnings = 0;



/* ****************************************************************************
 *
 * paWarningInit -
 */
void paWarningInit(void) {
  int ix;

  for (ix = 0; ix < (int)BA_VEC_SIZE(paWarning); ix++) {
    paWarning[ix].string = NULL;
    paWarning[ix].severity = PasNone;
  }

  paWarnings = 0;
}

/* ****************************************************************************
 *
 * paWarningAdd -
 */
void paWarningAdd(PaSeverity severity, char *txt) {
  static int ix    = 0;

  if (ix >= (int)BA_VEC_SIZE(paWarning)) {
    ix = 0;
  }
  if (paWarning[ix].string != NULL) {
    free(paWarning[ix].string);
  }
  paWarning[ix].string   = strdup(txt);
  paWarning[ix].severity = severity;

  // LM_W((paWarning[ix].string));
  ++ix;

  ++paWarnings;
}

