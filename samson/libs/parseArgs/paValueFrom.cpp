/* ****************************************************************************
 *
 * FILE			paArgs.c - source file for the parse arguments utility
 *
 * AUTHOR		Ken Zangelin
 *
 * HISTORY
 * $Log: paArgs.c,v $
 *
 */
#include <string.h>                   /* strcmp                              */

#include "parseArgs/paEnvVals.h"      /* paEnvName                           */
#include "parseArgs/paIterate.h"      /* paIterateInit, paIterateNext        */
#include "parseArgs/paPrivate.h"      /* paiList                             */
#include "parseArgs/paValueFrom.h"    /* Own interface                       */
#include "parseArgs/parseArgs.h"      /* PaFrom, PaArgument                  */



/* ****************************************************************************
 *
 * paValueFrom -
 */
PaFrom paValueFrom(char *oName) {
  PaiArgument *aP;

  paIterateInit();
  while ((aP = paIterateNext(paiList)) != NULL) {
    char envVarName[64];

    if (aP->option == NULL)
      continue; paEnvName(aP, envVarName);

    if (aP->option && (strcmp(oName, aP->option) == 0))
      return aP->from; else if (aP->envName && (strcmp(oName, envVarName) == 0))
      return aP->from;
  }

  return PafError;
}

