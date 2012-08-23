#include "baStd.h"               /* BA standard header file                   */
#include "logMsg/logMsg.h"       /* LM_ENTRY, LM_EXIT, ...                    */

#include "paBuiltin.h"           /* paBuiltin, paBuiltinNoOf                  */
#include "paIxLookup.h"          /* Own interface                             */
#include "paOptions.h"           /* paOptionsNoOf                             */
#include "parseArgs/parseArgs.h"  /* PaArgument                                */



/* ****************************************************************************
 *
 * paIxLookup -
 */
PaiArgument *paIxLookup(PaiArgument *paList, int ix) {
  int builtins = paBuiltinNoOf();

  if (ix < builtins) {
    return &paBuiltin[ix];
  } else if (ix < paOptionsNoOf(paList)) {
    return &paList[ix - builtins];
  }

  return NULL;
}

