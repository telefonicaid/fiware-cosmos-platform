/* ****************************************************************************
 *
 * FILE			paDefaultValues.c
 *
 * AUTHOR		Ken Zangelin
 *
 * HISTORY
 * $Log: $
 *
 */
#include <cstdlib>               /* C++ free()                               */
#include <stdio.h>               /* stderr, stdout, ...                      */
#include <string.h>              /* strncmp, strspn, ...                     */

#include "baStd.h"               /* BA standard header file                  */
#include "logMsg/logMsg.h"       /* LM_ENTRY, LM_EXIT, ...                   */

#include "paDefaultValues.h"     /* Own interface                            */
#include "paIterate.h"           /* paIterateInit, paIterateNext             */
#include "paPrivate.h"           /* paBuiltin                                */
#include "paTraceLevels.h"       /* LmtPaDefVal, LmtPaLimits, ...            */
#include "paWarning.h"           /* paWaringInit, paWarningAdd               */
#include "parseArgs/parseArgs.h"  /* PaArgument                               */



/* ****************************************************************************
 *
 * paDefaultValues - set default values, initially
 */
int paDefaultValues(PaiArgument *paList) {
  PaiArgument *aP;
  char w[512];

  LM_ENTRY();

  paIterateInit();
  while ((aP = paIterateNext(paList)) != NULL) {
    long long *defP;

    if (aP->def == PaNoDef) {
      continue;
    }
    aP->from = PafDefault;
    if (aP->type != PaString) {
      LM_T(LmtPaDefVal, ("setting default value for '%s' (0x%x)", aP->name, (int)aP->def));
    } else {
      LM_T(LmtPaDefVal, ("setting default value for '%s' (%s)", aP->name, (char *)aP->def));
    }
    defP = (long long *)&aP->def;

    switch (aP->type) {
      case PaInt:     *((int *)aP->varP) = *defP;                    break;
      case PaIntU:    *((int *)aP->varP) = *defP;                    break;
      case PaInt64:   *((long int *)aP->varP) = *defP;                    break;
      case PaIntU64:  *((unsigned long int *)aP->varP) = *defP;                    break;
      case PaChar:    *((char *)aP->varP) = (char)*defP;        break;
      case PaCharU:   *((char *)aP->varP) = (char)*defP;        break;
      case PaShort:   *((short *)aP->varP) = (short)*defP;        break;
      case PaShortU:  *((short *)aP->varP) = (short)*defP;        break;
      case PaBoolean: *((bool *)aP->varP) = (bool)*defP;        break;
      case PaFloat:   *((float *)aP->varP) = (float)*defP;        break;
      case PaDouble:  *((double *)aP->varP) = (double)*defP;        break;

      case PaString:
        if (aP->def) {
          if (((char *)aP->def)[0] != 0) {
            if ((char *)aP->varP != (char *)aP->def) {
              strcpy((char *)aP->varP, (char *)aP->def);
            }
          }
        } else {
          ((char *)aP->varP)[0] = 0;
        }
        break;

      default:
        sprintf(w, "type %d unknown for %s",
                aP->type, aP->name);
        PA_WARNING(PasProgrammingError, w);
        continue;
    }

    if (aP->type != PaString) {
      LM_T(LmtPaDefVal, ("default value for '%s' is set", aP->name));
    } else {
      LM_T(LmtPaDefVal, ("default value for '%s' is set to '%s'", aP->name, (char *)aP->varP));
    }
  }

  LM_EXIT();
  return 0;
}

