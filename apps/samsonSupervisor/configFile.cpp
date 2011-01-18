/* ****************************************************************************
*
* FILE                     configFile.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jan 17 2011
*
*/
#include <stdlib.h>             // atoi

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // LMT_*

#include "Process.h"            // Process
#include "processList.h"        // processAdd, ...
#include "configFile.h"         // Own interface



/* ****************************************************************************
*
* argsParse - 
*/
static int argsParse(char* line, char* host, char* process, char** args, int* argCount)
{
	int  ix;
	int  argIx = 0;
	
	LM_T(LMT_CONFIG_FILE, ("=============================================================="));
	LM_T(LMT_CONFIG_FILE, ("Parsing line: %s", line));

	/* 1. Remove comment part of line */
	ix = 0;
	while (line[ix] != 0)
	{
		if (line[ix] == '#')
		{
			line[ix] = 0;
			break;
		}

		ix += 1;
	}
	LM_T(LMT_CONFIG_FILE, ("w/o comments: %s", line));

	/* 2. Remove '\n' at end of line */
	if (line[strlen(line) - 1] == '\n')
		line[strlen(line) - 1] = 0;
	LM_T(LMT_CONFIG_FILE, ("w/o trailing newline: '%s'", line));


	/* 3. Eat leading whitespace */
	while (*line != 0)
	{
		if ((*line == ' ') || (*line == '\t'))
			++line;
		else
			break;
	}
	LM_T(LMT_CONFIG_FILE, ("w/o leading whitespace: '%s'", line));

	if (line[0] == 0)
		return -1;

	/* 4. Get 'host' */
	ix = 0;
	while (line[ix] != 0)
	{
		if (line[ix] == ' ')
		{
			line[ix] = 0;
			strcpy(host, line);
			line = &line[ix + 1];
			LM_T(LMT_CONFIG_FILE, ("Got host '%s'. line: '%s'", host, line));
			break;
		}
		ix += 1;
	}



	/* 5. Get 'process' */
	LM_T(LMT_CONFIG_FILE, ("Get process: '%s'", line));
	while (*line != 0)
	{
		if ((*line == ' ') || (*line == '\t'))
			++line;
		else
			break;
	}

    ix = 0;
    while (line[ix] != 0)
    {
        if (line[ix] == ' ')
        {
            line[ix] = 0;
            strcpy(process, line);
            line = &line[ix + 1];
			LM_T(LMT_CONFIG_FILE, ("Got process '%s'. line: '%s'", process, line));
            break;
        }
        ix += 1;
    }



	/* 6. Get rest of args */
	argIx = 0;
	while (*line != 0)
	{
		char* start;

		/* 7.1. Eat leading whitespace */
		LM_T(LMT_CONFIG_FILE, ("line: %s", line));
		while ((*line == ' ') || (*line == '\t'))
			++line;
		LM_T(LMT_CONFIG_FILE, ("line: %s", line));

		/* 7.2. Find next space (or newline) */
		start = line;
		LM_T(LMT_CONFIG_FILE, ("start: %s", start));
		while ((*line != ' ') && (*line != '\t') && (*line != '\n') && (*line != 0))
			++line;

		LM_T(LMT_CONFIG_FILE, ("line: %s", line));
		if (*line != 0)
		{
			*line = 0;
			++line;
			LM_T(LMT_CONFIG_FILE, ("line: %s", line));
		}

		LM_T(LMT_CONFIG_FILE, ("Got arg %d: '%s'. line: '%s'", argIx, start, line));
		args[argIx++] = strdup(start);
		LM_T(LMT_CONFIG_FILE, ("REST: '%s'", line));
	}

	*argCount = argIx;

	return 0;
}



/* ****************************************************************************
*
* configFileParse - 
*/
void configFileParse(char* cfPath)
{
	char    line[160];
	FILE*   fP;

	fP = fopen(cfPath, "r");
	if (fP == NULL)
		LM_X(1, ("opening '%s': %s", cfPath, strerror(errno)));

	while (fgets(line, sizeof(line), fP) != NULL)
	{
		char   host[32];
		char   process[32];
		char*  args[100];
		int    argCount;

		memset(args, 0, sizeof(args));
		LM_T(LMT_CONFIG_FILE, ("parsing line: %s", line));
		if (argsParse(line, host, process, args, &argCount) == 0)
		{
			LM_T(LMT_CONFIG_FILE, ("Adding process '%s' in %s with %d args", process, host, argCount));
			processAdd(process, host, args, argCount);
		}
	}
}
