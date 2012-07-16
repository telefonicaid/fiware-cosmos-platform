/* ****************************************************************************
*
* FILE                versionRequest.cpp - 
*
*
*
*
*/
#include <string>

#include "logMsg/logMsg.h"                  // LM_*

#include "traceLevels.h"                    // Trace levels for log msg library
#include "Format.h"                         // Format (XML, JSON)
#include "Verb.h"                           // Verb
#include "rest.h"                           // restReply
#include "globals.h"                        // version
#include "versionRequest.h"                 // Own interface

using namespace std;



/* ****************************************************************************
*
* versionRequest - 
*/
int versionRequest(int fd, Verb verb, Format format, int components, std::string* component)
{
	if (verb == GET)
		restReply(fd, format, 200, "version", version.c_str());
	else
	{
		allow = "GET";
		restReply(fd, format, 405, "error", "BAD VERB");
	}

	return 0;
}
