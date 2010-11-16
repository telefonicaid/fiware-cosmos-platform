#include "baStd.h"              /* BA standard header file                   */
#include "logMsg.h"             /* LM_ENTRY, LM_EXIT, ...                    */

#include "parseArgs.h"          /* PaArgument                                */
#include "paBuiltin.h"          /* paBuiltin, paBuiltinNoOf                  */
#include "paOptions.h"          /* paOptionsNoOf                             */
#include "paIxLookup.h"         /* Own interface                             */



/* ****************************************************************************
*
* paIxLookup - 
*/
PaArgument* paIxLookup(PaArgument* paList, int ix)
{
   int builtins = paBuiltinNoOf();

   if (ix < builtins)
      return &paBuiltin[ix];
   else if (ix < paOptionsNoOf(paList))
      return &paList[ix - builtins];

   return NULL;
}
