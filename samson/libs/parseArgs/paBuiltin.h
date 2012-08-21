#ifndef PA_BUILTIN_H
#define PA_BUILTIN_H

/* ****************************************************************************
 *
 * FILE                  paBuiltin.h
 *
 * AUTHOR                Ken Zangelin
 *
 */
#include "parseArgs/parseArgs.h"         /* PaArgument                                 */



/* ****************************************************************************
 *
 * builtin option variables
 */
extern char paHome[512];
extern bool paNoClear;
extern bool paClearAt;
extern bool paAssertAtExit;
extern int paKeepLines;
extern int paLastLines;
extern bool paUsageVar;
extern bool paEUsageVar;
extern bool paHelpVar;
extern bool paVersion;
extern char paUserName[64];
extern char paPwd[512];
extern char paColumns[128];
extern char paRows[128];
extern char paDisplay[128];
extern char paEditor[128];
extern char paLang[128];
extern char paPager[128];
extern char paPpid[128];
extern char paPrinter[128];
extern char paShell[128];
extern char paTerm[128];
extern char paSystem[128];
extern char paVisual[128];
extern char paLogDir[256];



/* ****************************************************************************
 *
 * paBuiltin - vector of builtin
 */
extern PaiArgument paBuiltin[];
extern int paBuiltins;



/* ****************************************************************************
 *
 * paBuiltinNoOf -
 */
extern int paBuiltinNoOf(void);



/* ****************************************************************************
 *
 * paBuiltinLookup -
 */
extern PaiArgument *paBuiltinLookup(char *option);



/* ****************************************************************************
 *
 * paBuiltinRemove
 */
extern int paBuiltinRemove(char *name);

#endif  // ifndef PA_BUILTIN_H
