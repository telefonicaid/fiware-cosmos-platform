#include "logMsg/logMsg.h"              // LM_*

#include "traceLevels.h"                // Trace levels for log msg library
#include "httpData.h"                   // Own interface



/* ****************************************************************************
*
* HTTP Header and Data variables
*/
char* contentType;
char* contentLength;
char* host;
char* userAgent;
char* accepts;

char*        httpData[100];
std::string  httpDataString;
int          httpDataLines = 0;



extern int lineSplit(char* buf, char** lineX);
/* ****************************************************************************
*
* httpDataParse - 
*/
bool httpDataParse(char** input, int lines, bool continued)
{
    int  ix;
	bool isData       = false;
	bool dataSeparate = false;

	LM_T(LmtInputLines, ("******************************************"));
	httpDataLines = 0;

	if (continued)
		isData = true;

	httpDataString = "";
	//
	// Skipping first line (GET/PUT/POST/DELETE)
	//
    for (ix = 1; ix < lines; ix++)
	{
	    // LM_T(LmtInputLines, ("input: '%s' (%x)", input[ix], input[ix]));

		if (input[ix][0] == 0)
		{
			isData = true; // next lines are data lines
			LM_T(LmtHttpData, ("Got an empty line - nextcoming lines are data"));
		}
		else if (isData)
		{
			LM_T(LmtHttpData, ("dataLine[%d]: '%s'", httpDataLines, input[ix]));
			httpData[httpDataLines++] = input[ix];
			httpDataString += input[ix];
		}
		else if (strncmp(input[ix], "Expect: 100-continue", 19) == 0)
		{
			dataSeparate = true;
			LM_T(LmtHttpHeader, ("Data comes in a separate package"));
		}
		else if (strncmp(input[ix], "Accept: ", 8) == 0)
		{
			accepts = &input[ix][8];
			LM_T(LmtHttpHeader, ("Accepts:          '%s'", accepts));
		}
		else if (strncmp(input[ix], "User-Agent: ", 12) == 0)
		{
			userAgent = &input[ix][12];
			LM_T(LmtHttpHeader, ("userAgent:        '%s'", userAgent));
		}
		else if (strncmp(input[ix], "Host: ", 6) == 0)
		{
			host = &input[ix][6];
			LM_T(LmtHttpHeader, ("Host:             '%s'", host));
		}
		else if (strncmp(input[ix], "Content-Length: ", 16) == 0)
		{
			contentLength = &input[ix][16];
			LM_T(LmtHttpHeader, ("Content-Length:   '%s'", contentLength));
		}
		else if (strncmp(input[ix], "Content-Type: ", 14) == 0)
		{
			contentType = &input[ix][14];
			LM_T(LmtHttpHeader, ("Content-Type:     '%s'", contentType));
		}
	}

	return dataSeparate;
}
