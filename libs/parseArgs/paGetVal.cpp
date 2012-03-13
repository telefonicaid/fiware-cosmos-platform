#include <stdio.h>              /* stderr, stdout, ...                       */
#include <stdlib.h>             /* strtol, atoi                              */

#include "baStd.h"              /* BA standard header file                   */
#include "logMsg/logMsg.h"             /* lmVerbose, lmDebug, ...                   */

#include "parseArgs/parseArgs.h"          /* progName                                  */
#include "paWarning.h"          /* paWarningAdd                              */
#include "paGetVal.h"           /* Own interface                             */



/* ****************************************************************************
*
* paGetVal - calculate the integer value of a string
*/
void* paGetVal(char* string, int* error)
{
	long      value;
	int       type;
    char      errorText[256];

    errorText[0] = 0;

	*error = PaOk;

    value = baStoi(string, &type, errorText);
    if (errorText[0] != 0)
	{
		PA_WARNING(PasBadValue, errorText);
		*error = type;
		return NULL;
	}
	
    return (void*) value;
}
