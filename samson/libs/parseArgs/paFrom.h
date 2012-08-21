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

#include "parseArgs/parseArgs.h" /* PaArgument                                */



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
