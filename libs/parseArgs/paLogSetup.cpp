#include <stdio.h>              /* stderr, stdout, ...                       */

#include "baStd.h"              /* BA standard header file                   */
#include "logMsg/logMsg.h"             /* lmVerbose, lmDebug, ...                   */

#include "paPrivate.h"          /* PaTypeUnion, config variables, ...        */
#include "paTraceLevels.h"      /* LmtPaEnvVal, ...                          */
#include "parseArgs/paConfig.h"           /* paConfigActions                           */
#include "paWarning.h"          /* paWaringInit, paWarningAdd                */
#include "paLogSetup.h"         /* Own interface                             */
#include <cstdlib>				/* C++ free(.)								 */



/* ****************************************************************************
*
* 
*/
int  lmFd   = -1;
int  lmSd   = -1;



/* ****************************************************************************
*
* paLmFdGet
*/
int paLmFdGet(void)
{
	return lmFd;
}



/* ****************************************************************************
*
* paLmSdGet
*/
int paLmSdGet(void)
{
	return lmSd;
}



/* ****************************************************************************
*
* paLogSetup - 
*/
int paLogSetup(void)
{
	LmStatus    s;
	char        w[512];

	if (paLogToFile)
	{
		s = lmPathRegister(paLogFilePath, paLogFileLineFormat, paLogFileTimeFormat, &lmFd);
		if (s != LmsOk)
		{
			sprintf(w, "lmPathRegister: %s", lmStrerror(s));
			PA_WARNING(PasLogFile, w);
			return -2;
		}
	}

	if (paLogToScreen)
	{
		int fd = 1;

		if (paLogScreenToStderr)
			fd = 2;

		s = lmFdRegister(fd, paLogScreenLineFormat, paLogScreenTimeFormat, "stdout", &lmSd);
		if (s != LmsOk)
		{
			sprintf(w, "lmFdRegister: %s", lmStrerror(s));
			PA_WARNING(PasLogFile, w);
			return -3;
		}
	}

	if (paLogToFile || paLogToScreen)
	{
		if ((s = lmInit()) != LmsOk)
		{
			sprintf(w, "lmInit: %s", lmStrerror(s));
			PA_WARNING(PasLogFile, w);
			return -4;
		}

		lmToDo    = false;
		lmVerbose = false;
		lmDebug   = false;
		/* lmBug     = false; */
		lmReads   = false;
		lmWrites  = false;

		lmTraceSet((char*) "");

		if (paLogToScreen && paLogScreenOnlyErrors)
			lmOnlyErrors(lmSd);
	}

	return 0;
}



