/* ****************************************************************************
*
* FILE			paArgs.c - source file for the parse arguments utility
*
* AUTHOR		Ken Zangelin
*
*
* TODO
* o OK to declare an option twice!
*   Must check option name collosions (also env vars)!
*
*
* HISTORY
* $Log: paArgs.c,v $
*
*/
#include <stdio.h>              /* stderr, stdout, ...                       */
#include <string.h>             /* strdup, strncmp                           */
#include <memory.h>             /* memset                                    */
#include <unistd.h>             /* getpid                                    */
#include <stdlib.h>             /* exit                                      */

#include "baStd.h"              /* BA standard header file                   */
#include "logMsg/logMsg.h"      /* lmVerbose, lmDebug, ...                   */

#include "paPrivate.h"          /* PaTypeUnion, config variables, ...        */
#include "paTraceLevels.h"      /* LmtPaEnvVal, ...                          */
#include "paUsage.h"            /* paUsage, paExtendedUsage, paHelp          */
#include "paConfig.h"           /* paConfigActions                           */
#include "paFullName.h"         /* paFullName                                */
#include "paBuiltin.h"          /* paBuiltin, paBuiltinNoOf                  */
#include "paEnvVals.h"          /* paEnvVals                                 */
#include "paRcFile.h"           /* paRcFileParse                             */
#include "paWarning.h"          /* paWaringInit, paWarningAdd                */
#include "paIterate.h"          /* paIterateInit, paIterateNext              */
#include "paDefaultValues.h"    /* paDefaultValues                           */
#include "paOptionsParse.h"     /* paOptionsParse                            */
#include "paLimitCheck.h"       /* paLimitCheck                              */
#include "paLogSetup.h"         /* paLogSetup                                */
#include "paIxLookup.h"         /* paIxLookup                                */
#include "paOptions.h"          /* paOptionsNoOf                             */
#include "parseArgs.h"          /* Own interface                             */



/* ****************************************************************************
*
* variables for user of this library
*/
char*        rcFileName;             /* needed for messages (and by lmLib) */
char         paCommandLine[1024];    /* entire command line                */
char         paResultString[60000];



/* ****************************************************************************
*
* optionNameDuplicated - 
*/
static bool optionNameDuplicated(char* name, PaArgument* paList, int start)
{
	int ix;
	int opts    = paOptionsNoOf(paList);
	int matches = 0;

	if (name == NULL)
		return false;
	if (name[0] == 0)
		return false;

	for (ix = start; ix < opts; ix++)
	{
		PaArgument* aP;

		if ((aP = paIxLookup(paList, ix)) == NULL)
			break;

		if (aP->option == NULL)
			continue;

		if (aP->removed == true)
			continue;

		if (aP->option[0] == 0)
			continue;

		if (strcmp(aP->option, " ") == 0)
			continue;

		if (strcmp(name, aP->option) == 0)
			++matches;
	}
	
	if (matches <= 1)
		return false;

	return true;
}



/* ****************************************************************************
*
* envNameDuplicated - 
*/
static bool envNameDuplicated(char* name, PaArgument* paList, int start)
{
	int   ix;
	int   opts    = paOptionsNoOf(paList);
	int   matches = 0;
	
	if (name == NULL)
		return false;
	if (name[0] == 0)
		return false;

	for (ix = start; ix < opts; ix++)
	{
		PaArgument* aP;
		char        envVarName[128];

		if ((aP = paIxLookup(paList, ix)) == NULL)
			break;

		if (aP->removed == true)
			continue;

		if (aP->envName == NULL)
			continue;

		paEnvName(aP, envVarName);
		
		if (strcmp(name, envVarName) == 0)
			++matches;
	}
	
	if (matches <= 1)
		return false;
	return true;
}



/* ****************************************************************************
*
* paArgInit - 
*/
static int paArgInit(PaArgument* paList)
{
	PaArgument* aP;
	int         parNo = 0;
	int         ix    = 0;

	paIterateInit();
	while ((aP = paIterateNext(paList)) != NULL)
	{
		char envVarName[128];

		/* Set argument to be unused */
		aP->used        = 0;
		aP->hasDefault  = true;
		aP->hasMinLimit = true;
		aP->hasMaxLimit = true;
		aP->aux         = 0;

		/* Set the value to come from nowhere */
		aP->from = PafUnchanged;

		if ((aP->option != NULL) && (aP->option[0] == 0))
			aP->option = NULL;

		if ((aP->envName != NULL) && (aP->envName[0] == 0))
			aP->envName = NULL;


		if ((aP->envName != NULL) && ((aP->envName[0] == ' ') || (aP->envName[0] == '\t')))
		{
			char w[512];

			strcpy(w, "found an item with var name starting with whitespace - forbidden");
			PA_WARNING(PasVarNameWs, w);
			continue;
		}

		/* Set the name of the argument */
		if (aP->option == NULL) 
		{
			if (aP->envName == NULL)
			{
				sprintf(aP->name, "par %d", ++parNo);
				aP->what = PawParameter;
			}
			else
			{
				char envVarName[128];

				sprintf(aP->name, "var %s", paEnvName(aP, envVarName));
				aP->what = PawVariable;
			}
		}
		else if ((aP->option[0] == ' ') && (aP->option[1] == 0))
		{
			++parNo;

			if (aP->envName == NULL)
			{
				sprintf(aP->name, "par %d", parNo);
				aP->what = PawParameter;
			}
			else
			{
				char envVarName[128];

				sprintf(aP->name, "var %s", paEnvName(aP, envVarName));
				aP->what = PawVariable | PawParameter;
			}
		}
		else
		{
			if (aP->envName == NULL)
			{
				sprintf(aP->name, "option '%s'", aP->option);
				aP->what = PawOption;
			}
			else
			{
				sprintf(aP->name, "option '%s'", aP->option);
				aP->what = PawOption | PawVariable;
			}
		}

		if (optionNameDuplicated((char*) aP->option, paList, ix) == true)
		{
			char w[512];

			sprintf(w, "%s (%s) duplicated in source code", aP->name, aP->description);
			PA_WARNING(PasOptionNameDuplicated, w);
			aP->removed = true;
		}		  

		paEnvName(aP, envVarName);
		
		if (envNameDuplicated(envVarName, paList, ix) == true)
		{
			char w[512];
			
			sprintf(w, "Env var '%s' (%s) duplicated in source code", envVarName, aP->description);
			PA_WARNING(PasEnvNameDuplicated, w);
			aP->removed = true;
		}		  

		++ix;
	}

	return 0;
}



/* ****************************************************************************
*
* paProgNameSet - 
*/
static char* paProgNameSet(char* pn, int levels, bool pid)
{
	char*        start;
	static char  pName[128];

	if (pn == NULL)
		return NULL;

	if (levels < 1)
		levels = 1;

	start = &pn[strlen(pn) - 1];
	while (start > pn)
	{
		if (*start == '/')
			levels--;
		if (levels == 0)
			break;
		--start;
	}

	if (*start == '/')
		++start;

	strncpy(pName, start, sizeof(pName));
	if (pid == true)
	{
		char  pid[8];
		strncat(pName, "_", sizeof(pName) - 1);
		sprintf(pid, "%d", (int) getpid());
		strncat(pName, pid, sizeof(pName) - 1);
	}

	return pName;
}


/* ****************************************************************************
*
* RETURN_ERROR - 
*/
#define RETURN_ERROR(s)              \
do                                   \
{                                    \
	PA_WARNING(PasSetup, (char*) s); \
	return -1;                       \
} while (0)



/* ****************************************************************************
*
* paParse - parse the argument list
*
*/
int paParse
(
	PaArgument*  paList,
	int          argC,
	char*        argV[],
	int          level,
	bool         pid
)
{
	char*  progNameCopy;
	int    ix;
	int    s;
	FILE*  fP;

	memset(paResultString, 0, sizeof(paResultString));

	/* **************************************************** */
	/* Creating flat command line string paCommandLine      */
	/*                                                      */
	if (argC > 1)
	{
		strncpy(paCommandLine, argV[1], sizeof(paCommandLine));
		for (ix = 2; ix < argC; ix++)
		{
			strncat(paCommandLine, " ", sizeof(paCommandLine) - 1);
			strncat(paCommandLine, argV[ix], sizeof(paCommandLine) - 1);
		}
	}


	/* ********************************************************* */
	/* Calling paConfig just to make sure paConfigInit is called */
	/*                                                           */
	paConfig("make sure paConfigInit is called", 0);









	// 362

	/* *************************** */
	/* Setting up the program name */
	/*                             */
	if (paProgName == NULL) /* hasn't been set with paConfig */
	{
		progNameCopy = strdup(argV[0]);
		progName     = strdup(paProgNameSet(argV[0], level, pid));
		paProgName   = strdup(progName);
	}
	else
	{
		progNameCopy = strdup(progName);
		progName     = strdup(progName);
	}



	/* ************************************************* */
	/* The rcFileName must not have any '/' in its name. */
	/* it should be called (by default) .<progname>rc    */
	/* So, first we must get a progName without '/' ...  */
	/*                                                   */
	if (paRcFileName == NULL)
	{
		char rName[128];

		sprintf(rName, ".%src", paProgNameSet(progNameCopy, 1, false));
		paRcFileName = strdup(rName);
	}
	free(progNameCopy);


	/* ****************************************************** */
	/* Initializing all subsystems                            */
	/*                                                        */
	if ((s = paLogSetup()) == -1)
		RETURN_ERROR("paLogSetup error");

	if ((s != -2) && ((s = paArgInit(paList)) == -1))
		RETURN_ERROR("paArgInit error");

	if ((s != -2) && ((s = paConfigActions(true)) == -1))
		RETURN_ERROR("paConfigActions");

	if ((s != -2) && ((s = paDefaultValues(paList)) == -1))
		RETURN_ERROR("paDefaultValues");

#if 0
	// Samson doesn't use paRcFileParse ...
	if ((s != -2) && ((s = paRcFileParse(paList)) == -1))
		RETURN_ERROR("paRcFileParse");
#endif

	if ((s != -2) && ((s = paEnvVals(paList)) == -1))
		RETURN_ERROR("paEnvVals");

	if ((s != -2) && ((s = paOptionsParse(paList, argV, argC)) == -1))
		RETURN_ERROR("paOptionsParse");

	if ((s != -2) && ((s = paLimitCheck(paList)) == -1))
		RETURN_ERROR("paLimitCheck");

	if ((s != -2) && ((s = paConfigActions(false)) == -1))
		RETURN_ERROR("paConfigActions");


	fP = NULL;
	if (paMsgsToStdout)
		fP = stdout;
	if (paMsgsToStderr)
		fP = stderr;

	if (paResultString[0] != 0)
	{
		if (fP != NULL)
			fprintf(fP, "%s\n", paResultString);
	}
	else if (paWarnings > 0)
	{
		int ix;

		if (fP != NULL)
		{
			fprintf(fP, "\nEntire command line: '%s'\n\n", paCommandLine);

			fprintf(fP, "--- %s warnings ---\n", progName);
			for (ix = 0; ix < paWarnings; ix++)
				fprintf(fP, "Severity % 2d: %s\n\n", paWarning[ix].severity, paWarning[ix].string);

			if (paUsageOnAnyWarning)
			  paUsage(paList);
			if (paResultString[0] != 0)
			  fprintf(fP, "%s\n", paResultString);

			exit(1);
		}
		else
		{
			char s[64000];

			sprintf(paResultString, "\nEntire command line options: '%s'\n\n", paCommandLine);
			
			sprintf(s, "--- %s warnings ---\n", progName);
			strncat(paResultString, s, sizeof(paResultString) - 1);

			for (ix = 0; ix < paWarnings; ix++)
			{
				sprintf(s, "Severity % 2d: %s\n\n", paWarning[ix].severity, paWarning[ix].string);
				strncat(paResultString, s, sizeof(paResultString) - 1);
			}

			strncat(paResultString, "\n\n", sizeof(paResultString) - 1);

			if (paUsageOnAnyWarning)
			{
			  printf("paUsageOnAnyWarning == true (2)\n");
			  paUsage(paList);
			}
		}
	}

	return 0;
}
