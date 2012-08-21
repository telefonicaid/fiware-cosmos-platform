#include <stdio.h>               /* sprintf                                   */

#include "paFullName.h"          /* Own interface                             */
#include "parseArgs/parseArgs.h"  /* PaArgument                                */



/* ****************************************************************************
 *
 * paFullName - fill in name and description in string string
 */
char *paFullName(char *string, PaiArgument *aP) {
  if ((aP->option == NULL) || (aP->option[0] == 0))
    sprintf(string, "variable %s", aP->description); else if (aP->type == PaBoolean)
    sprintf(string, "%s (%s)", aP->name, aP->description); else
    sprintf(string, "%s <%s>", aP->name, aP->description); return string;
}

