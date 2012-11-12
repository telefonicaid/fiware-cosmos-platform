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
