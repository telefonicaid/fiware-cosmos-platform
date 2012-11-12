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
#include "baStd.h"              /* BA standard header file                   */
#include "logMsg/logMsg.h"      /* LM_ENTRY, LM_EXIT, ...                    */

#include "parseArgs.h"          /* PaArgument                                */
#include "paBuiltin.h"          /* paBuiltin, paBuiltinNoOf                  */
#include "paLog.h"              /* PA_M, ...                                 */
#include "paOptions.h"          /* Own interface                             */



/* ****************************************************************************
*
* paOptionsNoOf - 
*/
int paOptionsNoOf(PaiArgument* paList)
{
	int ix   = 0;
	int opts = 0;

	if (paList == NULL)
		return paBuiltinNoOf();

	PA_M(("Counting args"));

	while (paList[ix].type != PaLastArg)
	{
		PA_M(("arg %d: '%s'", ix, paList[ix].option));
		if (paList[ix].removed != true)
			++opts;
		++ix;
		PA_M(("%d args so far", opts));
	}

	return opts + paBuiltinNoOf();
}
