#include "baStd.h"              /* BA standard header file                   */
#include "logMsg/logMsg.h"             /* LM_ENTRY, LM_EXIT, ...                    */

#include "parseArgs/parseArgs.h"          /* PaArgument                                */
#include "paBuiltin.h"          /* paBuiltin, paBuiltinNoOf                  */
#include "paOptions.h"          /* Own interface                             */



/* ****************************************************************************
*
* paOptionsNoOf - 
*/
int paOptionsNoOf(PaArgument* paList)
{
	int ix   = 0;
	int opts = 0;

	if (paList == NULL)
		return paBuiltinNoOf();

	while (paList[ix].type != PaLastArg)
	{
		if (paList[ix].removed != true)
			++opts;
		++ix;
	}

	return opts + paBuiltinNoOf();
}
