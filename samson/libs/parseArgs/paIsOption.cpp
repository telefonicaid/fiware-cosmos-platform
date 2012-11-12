/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
#include <string.h>             /* strncmp                                   */

#include "baStd.h"              /* BA standard header file                   */
#include "logMsg/logMsg.h"             /* LM_T                                      */

#include "parseArgs/parseArgs.h"          /* PaArgument, ...                           */
#include "paTraceLevels.h"      /* LmtXXX                                    */
#include "paBuiltin.h"          /* paBuiltin, paBuiltinNoOf                  */
#include "paIterate.h"          /* paIterateInit, paIterateNext              */
#include "paIsOption.h"         /* Own interface                             */
#include <cstdlib>				/* C++ free(.)								 */



/* ****************************************************************************
*
* paIsOption - is the string 'string' an option?
*/
bool paIsOption(PaiArgument* paList, char* string)
{
	int          len;
	PaiArgument* aP;

	LM_ENTRY();
	paIterateInit();
	while ((aP = paIterateNext(paList)) != NULL)
	{
		if ((aP->option == NULL) || (aP->option[0] == 0))
			continue;

		len = MAX(strlen(aP->option), strlen(string));

		if (strncmp(aP->option, string, len) == 0)
		{
			LM_EXIT();
			return true;
		}
	}

	LM_EXIT();
	return false;
}



