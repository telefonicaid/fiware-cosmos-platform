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

#endif // ifndef PA_WARNINGS_H
