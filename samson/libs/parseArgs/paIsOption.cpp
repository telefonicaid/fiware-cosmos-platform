#include <string.h>              /* strncmp                                   */

#include "baStd.h"               /* BA standard header file                   */
#include "logMsg/logMsg.h"       /* LM_T                                      */

#include "paBuiltin.h"           /* paBuiltin, paBuiltinNoOf                  */
#include "paIsOption.h"          /* Own interface                             */
#include "paIterate.h"           /* paIterateInit, paIterateNext              */
#include "paTraceLevels.h"      /* LmtXXX                                    */
#include "parseArgs/parseArgs.h"  /* PaArgument, ...                           */
#include <cstdlib>               /* C++ free(.)								 */



/* ****************************************************************************
 *
 * paIsOption - is the string 'string' an option?
 */
bool paIsOption(PaiArgument *paList, char *string) {
  int len;
  PaiArgument *aP;

  LM_ENTRY();
  paIterateInit();
  while ((aP = paIterateNext(paList)) != NULL) {
    if ((aP->option == NULL) || (aP->option[0] == 0)) {
      continue;
    }
    len = MAX(strlen(aP->option), strlen(string));

    if (strncmp(aP->option, string, len) == 0) {
      LM_EXIT();
      return true;
    }
  }

  LM_EXIT();
  return false;
}

