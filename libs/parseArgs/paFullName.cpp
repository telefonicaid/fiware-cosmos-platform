#include <stdio.h>              /* sprintf                                   */

#include "parseArgs/parseArgs.h"          /* PaArgument                                */
#include "paFullName.h"         /* Own interface                             */



/* ****************************************************************************
*
* paFullName - fill in name and description in string string
*/
char* paFullName(char* string, size_t max_length, PaiArgument* aP)
{
	if ((aP->option == NULL) || (aP->option[0] == 0))
		snprintf(string, max_length, "variable %s", aP->description);
	else if (aP->type == PaBoolean)
		snprintf(string, max_length, "%s (%s)", aP->name, aP->description);
	else
		snprintf(string, max_length, "%s <%s>", aP->name, aP->description);

	return string;
}
