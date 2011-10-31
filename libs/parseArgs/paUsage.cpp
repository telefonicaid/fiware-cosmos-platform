#include <stdlib.h>             /* system                                    */
#include <unistd.h>             /* getpid                                    */

#include "baStd.h"              /* BA standard header file                   */
#include "logMsg/logMsg.h"      /* lmVerbose, lmDebug, ...                   */

#include "parseArgs.h"          /* PaArgument                                */
#include "paPrivate.h"          /* PaTypeUnion, paBuiltins, ...              */
#include "paTraceLevels.h"      /* LmtPaApVals, LmtPaUsage, ...              */
#include "paFrom.h"             /* paFrom, paFromName                        */
#include "paConfig.h"           /* paHelpFile, paHelpText                    */
#include "paFullName.h"         /* paFullName                                */
#include "paBuiltin.h"          /* paBuiltin, paBuiltinNoOf                  */
#include "paIterate.h"          /* paIterateInit, paIterateNext              */
#include "paEnvVals.h"          /* paEnvName                                 */
#include "paParse.h"            /* progNameV                                 */
#include "paBuiltin.h"          /* paBuiltinNoOf                             */
#include "paUsage.h"            /* Own interface                             */



/* ****************************************************************************
*
* getApVals - 
*/
static void getApVals
(
	PaArgument*  aP,
	char*        defVal,
	char*        minVal,
	char*        maxVal,
	char*        realVal
)
{
	PaTypeUnion*  defP;
	PaTypeUnion*  minP;
	PaTypeUnion*  maxP;

	LM_T(LmtPaApVals, ("Fixing def, min, max, real values for %s", aP->name));

	if (aP->def == PaNoDef)
		aP->hasDefault = false;
	if (aP->min == PaNoLim)
		aP->hasMinLimit = false;
	if (aP->max == PaNoLim)
		aP->hasMaxLimit = false;

	defP = (PaTypeUnion*) &aP->def;
	minP = (PaTypeUnion*) &aP->min;
	maxP = (PaTypeUnion*) &aP->max;

	switch (aP->type)
	{
	case PaIList:
	case PaSList:
	case PaLastArg:
		return;

	case PaString:
		sprintf(defVal,  "'%s'", (defP->i != PaNoDef)? (char*) aP->def  : "no default");
		sprintf(minVal,  "'%s'", (minP->i != PaNoLim)? (char*) aP->min  : "no min limit");
		sprintf(maxVal,  "'%s'", (maxP->i != PaNoLim)? (char*) aP->max  : "no max limit");
		sprintf(realVal, "'%s'", (aP->varP != NULL)?   (char*) aP->varP : "no value");
		break;

	case PaBoolean:
		sprintf(defVal,  "%s", BA_FT(aP->def));
		sprintf(realVal, "%s", BA_FT(*((bool*) aP->varP)));
		aP->hasMinLimit = false;
		aP->hasMaxLimit = false;
		break;

	case PaInt:
		sprintf(defVal,  "%ld", aP->def);
		sprintf(minVal,  "%ld", aP->min);
		sprintf(maxVal,  "%ld", aP->max);
		sprintf(realVal, "%ld", *((long*) aP->varP));
		break;

	case PaIntU:
		sprintf(defVal,  "%lu", aP->def);
		sprintf(minVal,  "%lu", aP->min);
		sprintf(maxVal,  "%lu", aP->max);
		sprintf(realVal, "%lu", *((unsigned long*) aP->varP));
		break;

	case PaShort:
		sprintf(defVal,  "%ld", aP->def & 0xFFFF);
		sprintf(minVal,  "%ld", aP->min & 0xFFFF);
		sprintf(maxVal,  "%ld", aP->max & 0xFFFF);
		sprintf(realVal, "%d", (*((short*) aP->varP) & 0xFFFF));
		break;

	case PaShortU:
		sprintf(defVal,  "%lu", aP->def & 0xFFFF);
		sprintf(minVal,  "%lu", aP->min & 0xFFFF);
		sprintf(maxVal,  "%lu", aP->max & 0xFFFF);
		sprintf(realVal, "%u", (*((unsigned short*) aP->varP) & 0xFFFF));
		break;

	case PaChar:
		sprintf(defVal,  "%ld", aP->def & 0xFF);
		sprintf(minVal,  "%ld", aP->min & 0xFF);
		sprintf(maxVal,  "%ld", aP->max & 0xFF);
		sprintf(realVal, "%d", (*((char*) aP->varP) & 0xFF));
		break;

	case PaCharU:
		sprintf(defVal,  "%lu", aP->def & 0xFF);
		sprintf(minVal,  "%lu", aP->min & 0xFF);
		sprintf(maxVal,  "%lu", aP->max & 0xFF);
		sprintf(realVal, "%u", (*((unsigned char*) aP->varP) & 0xFF));
		break;

	case PaFloat:
		sprintf(defVal,  "%f", (float) aP->def);
		sprintf(minVal,  "%f", (float) aP->min);
		sprintf(maxVal,  "%f", (float) aP->max);
		sprintf(realVal, "%f", (*((float*) aP->varP)));
		break;

	case PaDouble:
		sprintf(defVal,  "%f", (double) aP->def);
		sprintf(minVal,  "%f", (double) aP->min);
		sprintf(maxVal,  "%f", (double) aP->max);
		sprintf(realVal, "%f", (*((double*) aP->varP)));
		break;
	}

	LM_T(LmtPaApVals, ("Get def(%s), min(%s), max(%s), real(%s) values for %s",
					   defVal, minVal, maxVal, realVal, aP->name));
}



/* ****************************************************************************
*
* paUsage - print synopsis (to file pointer or to paResultString)
*/
void paUsage(PaArgument* paList)
{
	char*        spacePad;
	char         string[512];
	char         s[1024];
	PaArgument*  aP;
	int          ix       = -1;
	int          builtins = paBuiltinNoOf();

	LM_T(LmtPaUsage, ("presenting usage"));

	spacePad = (char*) strdup(progName);
	memset(spacePad, 0x20202020, strlen(spacePad));  /* replace progName */

	sprintf(s, "Usage: %s ", progName);
	strncat(paResultString, s, sizeof(paResultString) - 1);

	paIterateInit();
	LM_T(LmtPaUsage, ("presenting usage"));
	while ((aP = paIterateNext(paList)) != NULL)
	{
		char  xName[512];

		++ix;
		if (ix < builtins)
			continue;

		if (aP->sort == PaHid)
			continue;

		if (PA_IS_OPTION(aP) && (aP->sort == PaOpt))
			sprintf(xName, "[%s]", paFullName(string, aP));
		else if (PA_IS_OPTION(aP) && (aP->sort == PaReq))
			sprintf(xName, "%s", paFullName(string, aP));
		else if (PA_IS_PARAMETER(aP) && (aP->sort == PaOpt))
			sprintf(xName, "(%s)", aP->description);
		else if (PA_IS_PARAMETER(aP) && (aP->sort == PaReq))
			sprintf(xName, "[%s]", aP->description);
		else
			strcpy(xName, "                    ");

		sprintf(s, " %s\n", xName);
		strncat(paResultString, s, sizeof(paResultString) - 1);
		sprintf(s, "%s        ", spacePad); /* 8 spaces for "Usage:  " */
		strncat(paResultString, s, sizeof(paResultString) - 1);
    }

    LM_T(LmtPaUsage, ("presenting usage"));
    strncat(paResultString, "\r", sizeof(paResultString) - 1);

	printf("%s\n", paResultString);
	exit(1);
}



/* ****************************************************************************
*
* paExtendedUsage - print extended synopsis
*/
void paExtendedUsage(PaArgument* paList)
{
	char*        spacePad;
	char         string[80];
	PaArgument*  aP;
	int          optNameMaxLen = 0;
	int          varNameMaxLen = 0;
	int          valsMaxLen    = 0;
	char         format[64];
	char         progNAME[128];
	bool         firstLine = true;

	sprintf(progNAME, "Usage: %s ", progName);
	spacePad = (char*) strdup(progNAME);
	memset(spacePad, 0x20202020, strlen(spacePad));  /* replace progNAME */

	paIterateInit();
	while ((aP = paIterateNext(paList)) != NULL)
	{
		char  name[128];
		char  vals[128];
		char  defVal[20];
		char  minVal[20];
		char  maxVal[20];
		char  realVal[80];

		/* 1. Option Name */
		memset(name, 0, sizeof(name));
		if (PA_IS_OPTION(aP) && (aP->sort == PaOpt))
			sprintf(name, "[%s]", paFullName(string, aP));
		else if (PA_IS_OPTION(aP) && (aP->sort == PaReq))
			sprintf(name, "%s", paFullName(string, aP));
		else if (PA_IS_PARAMETER(aP) && (aP->sort == PaOpt))
			sprintf(name, "(%s)", aP->description);
		else if (PA_IS_PARAMETER(aP) && (aP->sort == PaReq))
			sprintf(name, "[%s]", aP->description);
		optNameMaxLen = MAX(strlen(name), (unsigned int) optNameMaxLen);


		/* 2. Variable Name */
		memset(name, 0, sizeof(name));
		if (PA_IS_VARIABLE(aP))
		{
			paEnvName(aP, name);
			varNameMaxLen = MAX(strlen(name), (unsigned int) varNameMaxLen);
		}
		

		/* 3. Values */
		getApVals(aP, defVal, minVal, maxVal, realVal);
	
		if (aP->hasDefault == false)
			sprintf(name, "%s", realVal);
		else
			sprintf(name, "%s /%s/", realVal, defVal);

		if ((aP->hasMinLimit == false) && (aP->hasMaxLimit == false))
			sprintf(vals, "%s", name);
		else if (aP->hasMinLimit == false)
			sprintf(vals, "%s <= %s", name, maxVal);
		else if (aP->hasMaxLimit == false)
			sprintf(vals, "%s >= %s", name, minVal);
		else
			sprintf(vals, "%s <= %s <= %s", minVal, name, maxVal);

		valsMaxLen = MAX(strlen(vals), (unsigned int) valsMaxLen);
	}
	
	sprintf(format, "%%-%ds %%-%ds %%-%ds %%-%ds %%s\n",
			(int) strlen(progName) + 1,
			optNameMaxLen + 2,
			varNameMaxLen + 2,
			valsMaxLen + 2);

	paIterateInit();
	while ((aP = paIterateNext(paList)) != NULL)
	{
		char  optName[128];
		char  varName[128];
		char  vals[128];
		char  from[128];
		char  defVal[20];
		char  minVal[20];
		char  maxVal[20];
		char  realVal[80];
		char  s[512];

		if (aP->sort == PaHid)
			continue;

		/* 1. Option Name */
		if (PA_IS_OPTION(aP) && (aP->sort == PaOpt))
			sprintf(optName, "[%s]", paFullName(string, aP));
		else if (PA_IS_OPTION(aP) && (aP->sort == PaReq))
			sprintf(optName, "%s", paFullName(string, aP));
		else if (PA_IS_PARAMETER(aP) && (aP->sort == PaOpt))
			sprintf(optName, "(%s)", aP->description);
		else if (PA_IS_PARAMETER(aP) && (aP->sort == PaReq))
			sprintf(optName, "[%s]", aP->description);
		else
			strcpy(optName, " ");

	
		/* 2. variable name */
		if (PA_IS_VARIABLE(aP))
			paEnvName(aP, varName);
		else
			strcpy(varName, " ");


		/* 3. Limits */
		if ((aP->type != PaSList) && (aP->type != PaIList))
		{
			char valWithDef[128];
			char fromN[64];
			
			getApVals(aP, defVal, minVal, maxVal, realVal);

			if (aP->hasDefault == false)
				sprintf(valWithDef, "%s", realVal);
			else
				sprintf(valWithDef, "%s /%s/", realVal, defVal);

			if ((aP->hasMinLimit == false) && (aP->hasMaxLimit == false))
				sprintf(vals, "%s", valWithDef);
			else if (aP->hasMinLimit == false)
				sprintf(vals, "%s <= %s", valWithDef, maxVal);
			else if (aP->hasMaxLimit == false)
				sprintf(vals, "%s >= %s", valWithDef, minVal);
			else
				sprintf(vals, "%s <= %s <= %s",
						minVal, valWithDef, maxVal);

			sprintf(from, "  (%s)", paFromName(aP, fromN));
		}
		else
		{
			sprintf(vals, " ");
			sprintf(from, " ");
		}
	
		sprintf(s, format, (firstLine)? progNAME : spacePad, optName, varName, vals, from);
		strncat(paResultString, s, sizeof(paResultString) - 1);
	
		firstLine = false;
	}

	strncat(paResultString, "\r", sizeof(paResultString) - 1);
    
	free(spacePad);

	printf("%s\n", paResultString);
	exit(1);
}



typedef struct HelpVar
{
	const char*  varName;
	char*        varP;
} HelpVar;


static char usageString[800 * 200];
/* ****************************************************************************
*
* helpVar - 
*/
HelpVar helpVar[] = 
{
    { "$PROGNAME",  progNameV      },
    { "$USER",      paUserName     },
    { "$PWD",       paPwd          },
    { "$USAGE",     usageString    },
    { "$COLUMNS",   paColumns      },
    { "$ROWS",      paRows         },
    { "$DISPLAY",   paDisplay      },
    { "$EDITOR",    paEditor       },
    { "$LANG",      paLang         },
    { "$PAGER",     paPager        },
    { "$PPID",      paPpid         },
    { "$PID",       paPid          },
    { "$PRINTER",   paPrinter      },
    { "$SHELL",     paShell        },
    { "$TERM",      paTerm         },
    { "$SYSTEM",    paSystem       },
    { "$VISUAL",    paVisual       }
};



/* ****************************************************************************
*
* paVersionPrint - 
*/
void paVersionPrint(void)
{
	if (paManVersion)
		printf("%s\n", paManVersion);
	else
		printf("No MAN version\n");

	if (paManCopyright)
		printf("%s\n\n", paManCopyright);
	else
		printf("No MAN Copyright\n");

	if (paManAuthor)
		printf("%s\n", paManAuthor);
	else
		printf("No MAN Author\n");
}



/* ****************************************************************************
*
* paManUsage - 
*/
static void paManUsage(PaArgument* paList)
{
	PaArgument*  aP;

	paIterateInit();

    while ((aP = paIterateNext(paList)) != NULL)
	{
		if (aP->sort == PaHid)
			continue;
		if ((aP->what & PawOption) == 0)
		{
			printf("Skipping '%s' (what == %d)\n", aP->option, aP->what);
			continue;
		}

		printf("  %-20s %s\n", aP->option, aP->description);
	}
}



/* ****************************************************************************
*
* paManHelp - 
*/
static void paManHelp(PaArgument* paList)
{
	paExitOnUsage = false;

	if (paManSynopsis)
		printf("Usage: %s %s\n", paProgName, paManSynopsis);
	else
		printf("Usage: %s <no synopsis available>\n", paProgName);

	if (paManShortDescription)
		printf("%s\n", paManShortDescription);
	else
		printf("%s (short description) ...\n", paProgName);

	paManUsage(paList);

	if (paManDescription)
		printf("%s\n", paManDescription);
	else
		printf("<No man description available>\n");

	if (paManExitStatus)
		printf("Exit status:\n %s\n", paManExitStatus);
	else
		printf("Exit status:\n <no exit status available>\n");

	if (paManReportingBugs)
		printf("Report %s %s\n", paProgName, paManReportingBugs);
		
	fflush(stdout);
}



/* ****************************************************************************
*
* paHelp - print help text
*/
void paHelp(PaArgument* paList)
{
	LM_ENTRY();

	paManHelp(paList);
	exit(1);

#if 0
	pid_t  pid   = getpid();   

	sprintf(paPid, "%d", pid);

	memset(paResultString, 0, sizeof(paResultString));
	paExitOnUsage = false;
	paUsage(paList);
	strncpy(usageString, paResultString, sizeof(usageString));
	memset(paResultString, 0, sizeof(paResultString));
	
	if (paHelpFile != NULL)
	{
		char  s[512];
		FILE* fP;
		char  line[1024];
		char  start[512];
		char  end[512];

		LM_T(LmtHelp, ("Got help file '%s'", paHelpFile));

		sprintf(s, "----- %s Help -----\n", progName);
		strncat(paResultString, s, sizeof(paResultString) - 1);

		if ((fP = fopen(paHelpFile, "r")) == NULL)
		{
			sprintf(s, "error opening help file '%s': %s", paHelpFile, strerror(errno));
			strncat(paResultString, s, sizeof(paResultString) - 1);
			return;
		}
	
		while (fgets(line, sizeof(line), fP) != NULL)
		{
			int ix;
			int changes;

			do
			{
				changes = 0;

				for (ix = 0; ix < (int) BA_VEC_SIZE(helpVar); ix++)
				{
					char* tmp;

					if ((tmp = strstr(line, helpVar[ix].varName)) != NULL)
					{
						if (helpVar[ix].varP == NULL)
							continue;

						LM_T(LmtHelp, ("found variable '%s'", helpVar[ix].varName));
						++changes;

						strncpy(end, &tmp[strlen(helpVar[ix].varName)], sizeof(end));
						*tmp = 0;
						strncpy(start, line, sizeof(start));
						sprintf(line, "%s%s%s", start, helpVar[ix].varP, end);
					}
				}
			} while (changes != 0);

			strncat(paResultString, line, sizeof(paResultString) - 1);
		}

		fclose(fP);
		strncat(paResultString, 
				"\n---------------------------------------------\n",
				sizeof(paResultString) - 1);
	}
	else
		paUsage(paList);
#endif
}
