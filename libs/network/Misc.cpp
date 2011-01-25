/* ****************************************************************************
*
* FILE                     Misc.h - miscellaneous functions
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Nov 04 2010
*
*/
#include <stdio.h>              // FILE, popen, fgets, ...
#include <cstdlib>				// free(.)

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // LMT_*

#include "Misc.h"               // Own interface



namespace ss
{

/* ****************************************************************************
*
* wordClean - 
*/
char* wordClean(char* str)
{
	char* endP;

	while ((*str == ' ') || (*str == '\t'))
		++str;

	endP = str;
	while ((*endP != 0) && (*endP != ' ') && (*endP != '\t'))
		++endP;
	*endP = 0;

	return str;
}



/* ****************************************************************************
*
* ipGet -
*/
char* ipGet(void)
{
	char  line[80];
	char* ipP = (char*) "II.PP";
	FILE* fP;
	
	LM_M(("Calling popen(ifconfig | grep \"inet addr:\" ...)"));
	fP = popen("ifconfig | grep \"inet addr:\" | awk -F: '{ print $2 }' | awk '{ print $1 }'", "r");
	LM_M(("popen returned %p", fP));
	if (fgets(line, sizeof(line), fP) != NULL)
	{
		if (line[strlen(line) - 1] == '\n')
			line[strlen(line) - 1] = 0;
		ipP = wordClean(line);
		LM_T(LMT_CONFIG, ("new IP: %s", ipP));
	}

	fclose(fP);

	return strdup(ipP);
}

}
