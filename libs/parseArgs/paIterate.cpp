#include "baStd.h"              /* BA standard header file                   */
#include "logMsg/logMsg.h"             /* LM_ENTRY, LM_EXIT, ...                    */

#include "parseArgs/parseArgs.h"          /* PaArgument                                */
#include "paBuiltin.h"          /* paBuiltin, paBuiltinNoOf                  */
#include "paIterate.h"          /* Own interface                             */



/* ****************************************************************************
*
* static variables
*/
static int ix = 0;
static int builtins = -1;



/* ****************************************************************************
*
* paIterateInit - 
*/
void paIterateInit(void)
{
	ix       = 0;
	builtins = paBuiltinNoOf();
}



/* ****************************************************************************
*
* paIterateNext - 
*/
PaArgument* paIterateNext(PaArgument* paList)
{
	PaArgument* aP;

	if (builtins == -1)
		paIterateInit();

	do
	{
		if (ix < builtins)
			aP = &paBuiltin[ix];
		else if (paList != NULL)
			aP = &paList[ix - builtins];
		else
		  return NULL;

		++ix;
	} while (aP->removed == true);

	if (aP->type == PaLastArg)
		return NULL;

	return aP;
}
