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
#include "traceLevels.h"        // Trace Levels

#include "ports.h"              // CONTROLLER_PORT, WORKER_PORT
#include "Process.h"            // Process
#include "processList.h"        // processAdd, ...
#include "configFile.h"         // Own interface


static char* configFilePath = NULL;



/* ****************************************************************************
*
* configFileInit
*/
void configFileInit(const char* path)
{
	configFilePath = strdup(path);
}



/* ****************************************************************************
*
* argsParse - 
*/
static int argsParse(char* line, char* host, char* process, char** args, int* argCount)
{
	int  ix;
	int  argIx = 0;
	
	LM_T(LmtConfigFile, ("Parsing line: %s", line));

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
	LM_D(("w/o comments: %s", line));

	/* 2. Remove '\n' at end of line */
	if (line[strlen(line) - 1] == '\n')
		line[strlen(line) - 1] = 0;
	LM_D(("w/o trailing newline: '%s'", line));


	/* 3. Eat leading whitespace */
	while (*line != 0)
	{
		if ((*line == ' ') || (*line == '\t'))
			++line;
		else
			break;
	}
	LM_D(("w/o leading whitespace: '%s'", line));

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
			LM_T(LmtConfigFile, ("Got host '%s'. line: '%s'", host, line));
			break;
		}
		ix += 1;
	}



	/* 5. Get 'process' */
	LM_D(("Get process: '%s'", line));
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
			LM_T(LmtConfigFile, ("Got process '%s'. line: '%s'", process, line));
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
		LM_D(("line: %s", line));
		while ((*line == ' ') || (*line == '\t'))
			++line;
		LM_D(("line: %s", line));

		/* 7.2. Find next space (or newline) */
		start = line;
		LM_D(("start: %s", start));
		while ((*line != ' ') && (*line != '\t') && (*line != '\n') && (*line != 0))
			++line;

		LM_D(("line: %s", line));
		if (*line != 0)
		{
			*line = 0;
			++line;
			LM_D(("line: %s", line));
		}

		LM_T(LmtConfigFile, ("Got arg %d: '%s'. line: '%s'", argIx, start, line));
		args[argIx++] = strdup(start);
		LM_D(("REST: '%s'", line));
	}

	*argCount = argIx;

	return 0;
}



/* ****************************************************************************
*
* configFileParse - 
*/
int configFileParse(const char* processHost, const char* processName, int* argCP, char** argV)
{
	char    line[160];
	FILE*   fP;

	if (configFilePath == NULL)
		LM_X(1, ("configFilePath == NULL - please call configFileInit ..."));

	fP = fopen(configFilePath, "r");
	if (fP == NULL)
		LM_X(1, ("opening '%s': %s", configFilePath, strerror(errno)));

	while (fgets(line, sizeof(line), fP) != NULL)
	{
		char   host[32];
		char   process[32];

		LM_D(("parsing line: %s", line));

		if (argsParse(line, host, process, argV, argCP) == 0)
		{
			if (strcmp(host, processHost) != 0)
				continue;
			if (strcmp(process, processName) != 0)
				continue;

			LM_T(LmtConfigFile, ("Got %d parameters for %s@%s", *argCP, processName, processHost));
			fclose(fP);
			return 0;
		}
	}

	fclose(fP);
	LM_W(("No arguments found for process '%s' in machine '%s'", processName, processHost));
	return -1;
}



/* ****************************************************************************
*
* configFileParseByAlias - 
*/
int configFileParseByAlias(const char* alias, char* processHost, char* processName, int* argCP, char** argV)
{
	char    line[160];
	FILE*   fP;

	LM_T(LmtConfigFile, ("Looking up alias '%s'", alias));

	if (configFilePath == NULL)
		LM_X(1, ("configFilePath == NULL - please call configFileInit ..."));

	fP = fopen(configFilePath, "r");
	if (fP == NULL)
		LM_X(1, ("opening '%s': %s", configFilePath, strerror(errno)));

	while (fgets(line, sizeof(line), fP) != NULL)
	{
		LM_D(("parsing line: %s", line));

		if (argsParse(line, processHost, processName, argV, argCP) == 0)
		{
			for (int ix = 1; ix < *argCP; ix++)
			{
				if ((strcmp(argV[ix], alias) == 0) && (strcmp(argV[ix - 1], "-alias") == 0))
				{
					LM_T(LmtConfigFile, ("Got %d parameters for alias '%s' (%s@%s)", *argCP, alias, processName, processHost));

					fclose(fP);
					return 0;
				}
			}
		}
	}

	fclose(fP);
	LM_W(("No arguments found for process '%s' in machine '%s'", processName, processHost));
	return -1;
}
