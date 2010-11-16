#include <stdio.h>              /* sprintf                                  */

#include "baStd.h"              /* BA standard header file                  */

#include "paConfig.h"           /* config variables                          */
#include "parseArgs.h"          /* PaArgument                                */
#include "paWarning.h"          /* paWaringInit, paWarningAdd                */
#include "paIterate.h"          /* paIterateInit, paIterateNext              */
#include "paEnvVals.h"          /* paEnvName                                 */
#include "paBuiltin.h"          /* Own interface                             */



/* ****************************************************************************
*
* option variables
*/
bool        paUsageVar;     /* the -u options (-u, -h, -?, -help) */
bool        paEUsageVar;    /* the -U option                      */
bool        paHelpVar;      /* the -help option                   */
char        paHome[512];
bool        paNoClear;
bool        paClearAt;
int         paKeepLines;
int         paLastLines;
static int  dummy;
char        paUserName[64];
char        paPwd[512];
char        paColumns[128];
char        paRows[128];
char        paDisplay[128];
char        paEditor[128];
char        paLang[128];
char        paPager[128];
char        paPpid[128];
char        paPrinter[128];
char        paShell[128];
char        paTerm[128];
char        paSystem[128];
char        paVisual[128];



#define T (int) true
#define F (int) false
/* ****************************************************************************
*
* paBuiltin - 
*/
PaArgument paBuiltin[] =
{
 { 
    "--", 
    &dummy,
    NULL,
    PaBool,
    PaHid,
    false,
    true,
    false,
    "X delimiter"
 },

 { "-U",    &paEUsageVar, NULL,         PaBool,PaOpt,  F,    T,    F, "extended usage"       },
 { "-u",    &paUsageVar,  NULL,         PaBool,PaOpt,  F,    T,    F, "usage"                },
 { "-h",    &paUsageVar,  NULL,         PaBool,PaOpt,  F,    T,    F, "usage"                },
 { "-help", &paHelpVar,   NULL,         PaBool,PaOpt,  F,    T,    F, "show help"            },
 { "--help",&paHelpVar,   NULL,         PaBool,PaOpt,  F,    T,    F, "show help"            },
 { "-home", paHome,       "!HOME",      PaStr, PaHid,  0, PaNL, PaNL, "home directory"       },
 { "",      paUserName,   "!USER",      PaStr, PaHid,  0, PaNL, PaNL, "user name"            },
 { "",      paPwd,        "!PWD",       PaStr, PaHid,  0, PaNL, PaNL, "current dir"          },
 { "",      paColumns,    "!COLUMNS",   PaStr, PaHid,  0, PaNL, PaNL, "columns"              },
 { "",      paRows,       "!ROWS",      PaStr, PaHid,  0, PaNL, PaNL, "rows"                 },
 { "",      paDisplay,    "!DISPLAY",   PaStr, PaHid,  0, PaNL, PaNL, "display"              },
 { "",      paEditor,     "!EDITOR",    PaStr, PaHid,  0, PaNL, PaNL, "editor"               },
 { "",      paLang,       "!LANG",      PaStr, PaHid,  0, PaNL, PaNL, "language"             },
 { "",      paPager,      "!PAGER",     PaStr, PaHid,  0, PaNL, PaNL, "pager"                },
 { "",      paPpid,       "!PPID",      PaStr, PaHid,  0, PaNL, PaNL, "parent process id"    },
 { "",      paPrinter,    "!PRINTER",   PaStr, PaHid,  0, PaNL, PaNL, "printer"              },
 { "",      paShell,      "!SHELL",     PaStr, PaHid,  0, PaNL, PaNL, "shell"                },
 { "",      paTerm,       "!TERM",      PaStr, PaHid,  0, PaNL, PaNL, "terminal"             },
 { "",      paSystem,     "!SYSTEM",    PaStr, PaHid,  0, PaNL, PaNL, "system"               },
 { "",      paVisual,     "!VISUAL",    PaStr, PaHid,  0, PaNL, PaNL, "visual"               },
 { "-t",    paTraceV,     "TRACE",      PaStr, PaOpt,  0, PaNL, PaNL, "trace level"          },
 { "-v",    &paVerbose,   "VERBOSE",    PaBool,PaOpt,  F,    T,    F, "verbose mode"         },
 { "-d",    &paDebug,     "DEBUG",      PaBool,PaOpt,  F,    T,    F, "debug mode"           },
 { "-r",    &paReads,     "READS",      PaBool,PaOpt,  F,    T,    F, "reads mode"           },
 { "-w",    &paWrites,    "WRITES",     PaBool,PaOpt,  F,    T,    F, "writes mode"          },
 { "-F",    &paFix,       "FIX",        PaBool,PaOpt,  F,    T,    F, "fixes mode"           },
 { "-B",    &paBug,       "BUGS",       PaBool,PaOpt,  F,    T,    F, "bugs mode"            },
 { "-b",    &paBuf,       "BUFS",       PaBool,PaOpt,  F,    T,    F, "buf mode"             },
 { "-?",    &paDoubt,     "DOUBT",      PaBool,PaOpt,  F,    T,    F, "doubts mode"          },
 { "-lmnc", &paNoClear,   "NO_CLEAR",   PaBool,PaOpt,  F,    T,    F, "don't clear log file" },
 { "-lmca", &paClearAt,   "CLEAR_AT",   PaInt, PaOpt, -1, PaNL, PaNL, "clear at lines"       },
 { "-lmkl", &paKeepLines, "KEEP_LINES", PaInt, PaOpt, -1, PaNL, PaNL, "clear 'keep lines'"   },
 { "-lmll", &paLastLines, "LAST_LINES", PaInt, PaOpt, -1, PaNL, PaNL, "clear 'last lines'"   },

 PA_END_OF_ARGS
};

#undef T
#undef F



/* ****************************************************************************
*
* BuiltinNoOf - 
*/
int paBuiltinNoOf(void)
{
	int ix = 0;

	if (paUseBuiltins == false)
		return 0;

	while (paBuiltin[ix].type != PaLastArg)
		++ix;

	return ix;
}



/* ****************************************************************************
*
* paBuiltinRemove
*/
int paBuiltinRemove(char* name)
{
	PaArgument* aP;

	/* 1. lookup aP->option or aP->variable          */
	/* 2. if found - mark the aP as PaRemoved        */
	/* 3. Then paConfig needs the actions to do it   */

	paIterateInit();
	while ((aP = paIterateNext(NULL)) != NULL)
	{
		char envVarName[128];

		paEnvName(aP, envVarName);
		if (((aP->option) && (strcmp(name, aP->option) == 0))
		|| ((aP->envName) && (strcmp(name, envVarName) == 0)))
		{
			aP->removed = true;
			break;
		}
	}

	if (aP == NULL)
	{
		char w[512];

		sprintf(w, "cannot remove builtin '%s' - not found", name);
		PA_WARNING(PasBuiltinRemove, w);
		return 1;
	}

	return 0;
}
