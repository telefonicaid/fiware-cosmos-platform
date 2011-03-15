/* ****************************************************************************
*
* FILE			paConfig.c - configuration for parseArgs library
*
* AUTHOR		Ken Zangelin
*
* HISTORY
* $Log: paArgs.c,v $
*
*
*/
#include <stdlib.h>             /* free, ...                                  */
#include <string.h>             /* strdup, ...                                */

#include "logMsg.h"             /* lmTraceSet                                */

#include "paPrivate.h"          /* PaTypeUnion, config variables, ...        */
#include "paTraceLevels.h"      /* LmtPaDefaultValues, ...                   */
#include "paWarning.h"          /* paWaringInit, paWarningAdd                */
#include "paBuiltin.h"          /* paBuiltinRemove                           */
#include "paLog.h"              /* log macros to debug paConfig              */
#include "paConfig.h"           /* Own interface                             */



/* ****************************************************************************
*
* Configurable variables
*
* PROBLEM
* What do I do with default values for stings?
* if (p != NULL) free(p) ... 
*
* I might create a defaultVariable for each of the strings
*/
bool       paUseBuiltins         = true;
bool       paExitOnError         = true;
bool       paExitOnUsage         = true;
bool       paPrintErrorsOnStderr = true;
char*      paPrefix              = NULL;
int        paVersion             = 0;
char*      paVersionString       = NULL;
char*      paExecVersion         = NULL;
char*      paTraceInfoAtEol      = NULL;
char*      paProgName            = NULL;
char*      paRcFileName          = NULL;
char*      paRcFileDir           = NULL;
char*      paGenericRcDir        = NULL;

/* lmLib configuration variables */
bool       paLogToFile           = false;
bool       paLogToScreen         = false;
bool       paLogScreenToStderr   = false;
bool       paLogScreenOnlyErrors = false;

char*      paLogFilePath         = NULL;
char*      paLogFileLineFormat   = NULL;
char*      paLogFileTimeFormat   = NULL;
char*      paLogScreenLineFormat = NULL;
char*      paLogScreenTimeFormat = NULL;

bool       paLogClearing         = false;
char*      paHelpFile            = NULL;
char*      paHelpText            = NULL;
bool       paUsageOnAnyWarning   = false;



/* Debug setting variables */
char       paTraceV[1024];
char*      paTracelevels         = NULL;

bool       paVerbose             = false;
bool       paDebug               = false;
bool       paHidden              = false;
bool       paToDo                = false;
bool       paReads               = false;
bool       paWrites              = false;
bool       paFix                 = false;
bool       paBug                 = false;
bool       paBuf                 = false;
bool       paDoubt               = false;

bool       paMsgsToStdout        = true;
bool       paMsgsToStderr        = false;
char       paPid[16];



/* ****************************************************************************
*
* PaVars - 
*/
typedef struct PaVars
{
	char*      name;
	int        value;
	int        defVal;
	PaType     type;
	bool       preTreated;  /* all builtins for example */
	PavCb      cb;
	char*      info;
} PaVars;



/* ****************************************************************************
*
* paConfigInit - 
*/
static void paConfigInit(void)
{
	paWarningInit();

	if (paPrefix)
		paPrefix              = strdup(paPrefix);
	if (paProgName)
		paProgName            = strdup(paProgName);
	if (paRcFileName)
		paRcFileName          = strdup(paRcFileName);
	if (paRcFileDir)
		paRcFileDir           = strdup(paRcFileDir);
	if (paGenericRcDir)
		paGenericRcDir        = strdup(paGenericRcDir);
	if (paVersionString)
		paVersionString       = strdup(paVersionString);
	if (paExecVersion)
		paExecVersion         = strdup(paExecVersion);
	if (paHelpFile)
		paHelpFile            = strdup(paHelpFile);
	if (paHelpText)
		paHelpText            = strdup(paHelpText);
	if (paTracelevels)
		paTracelevels         = strdup(paTracelevels);


	if (paTraceInfoAtEol)
		paTraceInfoAtEol      = strdup(paTraceInfoAtEol);
	else
	   paTraceInfoAtEol      = strdup("#");

	if (paLogFilePath)
		paLogFilePath         = strdup(paLogFilePath);
	else
		paLogFilePath         = strdup("/tmp");


	if (paLogFileLineFormat)
		paLogFileLineFormat   = strdup(paLogFileLineFormat);
	else
		paLogFileLineFormat   = strdup("DEF");

	if (paLogFileTimeFormat)
		paLogFileTimeFormat   = strdup(paLogFileTimeFormat);
	else
		paLogFileTimeFormat   = strdup("DEF");

	if (paLogScreenLineFormat)
		paLogScreenLineFormat = strdup(paLogScreenLineFormat);
	else
		paLogScreenLineFormat = strdup("DEF");

	if (paLogScreenTimeFormat)
		paLogScreenTimeFormat = strdup(paLogScreenTimeFormat);
	else
		paLogScreenTimeFormat = strdup("DEF");

	/* Should all these be freed after paParse finishes? */
}



/* ****************************************************************************
*
* paConfig - 
*/
int paConfig(const char* item, void* value)
{
	static int firstTime = 0;
	long        val       = (long) value;

	PA_M(("setting value for item '%s'", item));

	if (firstTime == 0)
		paConfigInit();
	firstTime = 1;

	if (strcmp(item, "help file") == 0)
		paHelpFile = strdup((char*) val);
	else if (strcmp(item, "msgs to stdout") == 0)
		paMsgsToStdout = (bool) val;
	else if (strcmp(item, "msgs to stderr") == 0)
		paMsgsToStderr = (bool) val;
	else if (strcmp(item, "usage and exit on any warning") == 0)
		paUsageOnAnyWarning = (bool) val;
	else if (strcmp(item, "remove builtin") == 0)
		paBuiltinRemove((char*) value);
	else if (strcmp(item, "builtins") == 0)
		paUseBuiltins = (bool) val;
	else if (strcmp(item, "exit on error") == 0)
		paExitOnError = (bool) val;
	else if (strcmp(item, "exit on usage") == 0)
		paExitOnUsage = (bool) val;
	else if (strcmp(item, "print errors") == 0)
		paPrintErrorsOnStderr = (bool) val;
	else if (strcmp(item, "prefix") == 0)
	{
		if (paPrefix != NULL)
			free(paPrefix);
		paPrefix = strdup((char*) val);
	}
	else if (strcmp(item, "prog name") == 0)
	{
		if (paProgName != NULL)
			free(paProgName);
		paProgName = strdup((char*) val);
	}
	else if (strcmp(item, "rc file") == 0)
	{
		if (paRcFileName != NULL)
			free(paRcFileName);
		paRcFileName = strdup((char*) val);
	}
	else if (strcmp(item, "rc dir") == 0)
	{
		if (paRcFileDir != NULL)
			free(paRcFileDir);
		paRcFileDir = strdup((char*) val);
	}
	else if (strcmp(item, "rc generic dir") == 0)
	{
		if (paGenericRcDir != NULL)
			free(paGenericRcDir);
		paGenericRcDir = strdup((char*) val);
	}
	else if (strcmp(item, "trace levels") == 0)
	{
		if (paTracelevels != NULL)
			free(paTracelevels);
		paTracelevels = strdup((char*) value);
	}
	else if (strcmp(item, "verbose mode") == 0)
		paVerbose = (bool) val;
	else if (strcmp(item, "debug mode") == 0)
		paDebug = (bool) val;
	else if (strcmp(item, "hidden mode") == 0)
		paHidden = (bool) val;
	else if (strcmp(item, "toDo mode") == 0)
		paToDo = (bool) val;
	else if (strcmp(item, "reads mode") == 0)
		paReads = (bool) val;
	else if (strcmp(item, "writes mode") == 0)
		paWrites = (bool) val;
	else if (strcmp(item, "fix mode") == 0)
		paFix = (bool) val;
	else if (strcmp(item, "bug mode") == 0)
		paBug = (bool) val;
	else if (strcmp(item, "buf mode") == 0)
		paBuf = (bool) val;
	else if (strcmp(item, "doubt mode") == 0)
		paDoubt = (bool) val;
	else if (strcmp(item, "version") == 0)
	{
		if (paExecVersion != NULL)
			free(paExecVersion);
		paExecVersion = strdup((char*) value);
	}
	else if (strcmp(item, "log to file") == 0)
		paLogToFile = (bool) val;
	else if (strcmp(item, "log to screen") == 0)
	{
		if ((bool) val == true)
		{
			paLogToScreen  = true;
			paMsgsToStdout = true;
		}
		else if ((bool) val == false)
			paLogToScreen = false;
		else if (strcmp((char*) value, "only errors") == 0)
		{
			paLogToScreen         = true;
			paLogScreenOnlyErrors = true;
		}
	}
	else if (strcmp(item, "log to stderr") == 0)
		paLogScreenToStderr = (bool) val;
	else if (strcmp(item, "log file") == 0)
	{
		if (paLogFilePath != NULL)
			free(paLogFilePath);
		paLogFilePath = strdup((char*) value);
	}
	else if (strcmp(item, "log file line format") == 0)
	{
		if (paLogFileLineFormat != NULL)
			free(paLogFileLineFormat);
		paLogFileLineFormat = strdup((char*) value);
	}
	else if (strcmp(item, "log file time format") == 0)
	{
		if (paLogFileTimeFormat != NULL)
			free(paLogFileTimeFormat);
		paLogFileTimeFormat = strdup((char*) value);
	}
	else if (strcmp(item, "screen line format") == 0)
	{
		if (paLogScreenLineFormat != NULL)
			free(paLogScreenLineFormat);
		paLogScreenLineFormat = strdup((char*) value);
	}
	else if (strcmp(item, "screen time format") == 0)
	{
		if (paLogScreenTimeFormat != NULL)
			free(paLogScreenTimeFormat);
		paLogScreenTimeFormat = strdup((char*) value);
	}
	else if (strcmp(item, "make sure paConfigInit is called") == 0)
		;
	else
	{
		char w[256];

		sprintf(w, "paConfig command '%s' not recognized", item);
		PA_WARNING(PasNoSuchCommand, w);
	}

	return 0;
}


/* ****************************************************************************
*
* paConfigActions - 
*/
int paConfigActions(bool preTreat)
{
	lmVerbose = paVerbose;
	lmDebug   = paDebug;
	lmHidden  = paHidden;
	lmToDo    = paToDo;
	lmReads   = paReads;
	lmWrites  = paWrites;
	lmFix     = paFix;
	lmBug     = paBug;
	lmDoubt   = paDoubt;
	lmBuf     = paBuf;

	if (preTreat)
		lmTraceSet(paTracelevels);
	else
	{
		LM_ENTRY();
		lmTraceSet(paTraceV);
		if (paNoClear == true)
			lmDontClear();
		
		if ((paClearAt != -1) || (paKeepLines != -1) || (paLastLines != -1))
		{
			/* logMsg must be changed to not change -1 values */
			lmClearAt(paClearAt, paKeepLines, paLastLines);
		}

		LM_T(LmtPaConfigAction, ("setting trace levels to '%s'", paTraceV));
		LM_EXIT();
	}

	return 0;
}



/* ****************************************************************************
*
* paConfigCleanup - 
*/
void paConfigCleanup(void)
{
	if (paPrefix)
		free(paPrefix);
	if (paProgName)
		free(paProgName);
	if (paRcFileName)
		free(paRcFileName);
	if (paRcFileDir)
		free(paRcFileDir);
	if (paGenericRcDir)
		free(paGenericRcDir);
	if (paVersionString)
		free(paVersionString);
	if (paExecVersion)
		free(paExecVersion);
	if (paHelpFile)
		free(paHelpFile);
	if (paHelpText)
		free(paHelpText);
	if (paTracelevels)
		free(paTracelevels);
	if (paTraceInfoAtEol)
		free(paTraceInfoAtEol);
	if (paLogFilePath)
		free(paLogFilePath);
	if (paLogFileLineFormat)
		free(paLogFileLineFormat);
	if (paLogFileTimeFormat)
		free(paLogFileTimeFormat);
	if (paLogScreenLineFormat)
		free(paLogScreenLineFormat);
	if (paLogScreenTimeFormat)
		free(paLogScreenTimeFormat);
}
