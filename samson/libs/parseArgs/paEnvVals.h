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
