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

