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
#include <string.h>              /* strcmp                                    */

#include "parseArgs/parseArgs.h" /* PaFrom, PaArgument                        */
#include "paIterate.h"           /* paIterateInit, paIterateNext              */
#include "paEnvVals.h"           /* paEnvName                                 */
#include "paValueFrom.h"         /* Own interface                             */



/* ****************************************************************************
*
* paValueFrom - 
*/
PaFrom paValueFrom(PaArgument* paList, char* oName)
{
	PaArgument* aP;

	paIterateInit();
	while ((aP = paIterateNext(paList)) != NULL)
	{
		char envVarName[64];

		if (aP->option == NULL)
			continue;

		paEnvName(aP, envVarName);

		if (aP->option && (strcmp(oName, aP->option) == 0))
			return aP->from;
		else if (aP->envName && (strcmp(oName, envVarName) == 0))
			return aP->from;
	}  

	return PafError;
}
