#include <cstdlib>              /* C++ free(.)								 */
#include <stdio.h>              /* stderr, stdout, ...                       */

#include "baStd.h"              /* BA standard header file                   */
#include "logMsg/logMsg.h"      /* lmVerbose, lmDebug, ...                   */

#include "paBuiltin.h"          /* paLogDir                                  */
#include "paLogSetup.h"         /* Own interface                             */
#include "paPrivate.h"          /* PaTypeUnion, config variables, ...        */
#include "paTraceLevels.h"      /* LmtPaEnvVal, ...                          */
#include "paWarning.h"          /* paWaringInit, paWarningAdd                */
#include "parseArgs/paConfig.h"  /* paConfigActions                           */



/* ****************************************************************************
 *
 *
 */
int lmFd   = -1;
int lmSd   = -1;



/* ****************************************************************************
 *
 * paLmFdGet
 */
int paLmFdGet(void) {
  return lmFd;
}

/* ****************************************************************************
 *
 * paLmSdGet
 */
int paLmSdGet(void) {
  return lmSd;
}

/* ****************************************************************************
 *
 * paLogSetup -
 */
extern char *paExtraLogSuffix;
int paLogSetup(void) {
  LmStatus s = LmsOk;
  char w[512];

  // printf("In paLogSetup. paLogToFile == %s\n", (paLogToFile == true)? "true" : "false");

  if (paLogToFile == true) {
    // printf("paLogDir == '%s'\n", paLogDir);
    if (paLogDir[0] != 0)
      // printf("Using paLogDir '%s'", paLogDir);
      s = lmPathRegister(paLogDir, paLogFileLineFormat, paLogFileTimeFormat, &lmFd); else
      // printf("Using paLogFilePath: '%s'\n", paLogFilePath);
      s = lmPathRegister(paLogFilePath, paLogFileLineFormat, paLogFileTimeFormat, &lmFd); if (s != LmsOk) {
      sprintf(w, "lmPathRegister: %s", lmStrerror(s));
      PA_WARNING(PasLogFile, w);
      return -2;
    }
  }

  if (paLogToScreen) {
    int fd = 1;

    if (paLogScreenToStderr)
      fd = 2; s = lmFdRegister(fd, paLogScreenLineFormat, paLogScreenTimeFormat, "stdout", &lmSd);
    if (s != LmsOk) {
      sprintf(w, "lmFdRegister: %s", lmStrerror(s));
      PA_WARNING(PasLogFile, w);
      return -3;
    }
  }

  if (paLogToFile || paLogToScreen || lmNoTracesToFileIfHookActive) {
    if ((s = lmInit()) != LmsOk) {
      sprintf(w, "lmInit: %s", lmStrerror(s));
      PA_WARNING(PasLogFile, w);
      return -4;
    }

    lmToDo     = false;
    lmVerbose  = false;
    lmVerbose2 = false;
    lmVerbose3 = false;
    lmVerbose4 = false;
    lmVerbose5 = false;
    lmDebug    = false;
    /* lmBug     = false; */
    lmReads    = false;
    lmWrites   = false;

    lmTraceSet((char *)"");

    if (paLogToScreen && paLogScreenOnlyErrors)
      lmOnlyErrors(lmSd);
  }

  return 0;
}

