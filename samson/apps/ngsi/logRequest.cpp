/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
/* ****************************************************************************
*
* FILE                logRequest.cpp - 
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
#include "logRequest.h"                     // Own interface

using namespace std;



/* ****************************************************************************
*
* traceRequest - 
*
* DELETE log/trace          - reset ALL trace levels
* DELETE log/trace/a        - reset trace level 'a'
* DELETE log/trace/a,b-c    - reset trace level 'a' and the interval 'b' - 'c'
* PUT    log/trace/a        - set trace level 'a'
* PUT    log/trace/a,b-c    - set trace level 'a' and the interval 'b' - 'c'
*
*/
static int traceRequest(int fd, Verb verb, Format format, int components, std::string* component)
{
	LM_T(LmtLogRequest, ("verb: %s, components: %d, %s", verbName(verb), components, component[0].c_str()));
	if (verb == DELETE)
	{
		if (components == 2)
		{
			lmTraceSet(NULL);
			restReply(fd, format, 200, "trace", "all trace levels off");
			return 0;
		}
		else if (components == 3)
		{
			const char* s = component[2].c_str();

			if (strspn(s, "0123456789-'") == strlen(s))
			{
				char tLevels[256];
				char reply[256];

				lmTraceSub(s);
				lmTraceGet(tLevels);
				snprintf(reply, sizeof(reply), "new trace levels: '%s'", tLevels);
				restReply(fd, format, 200, "trace", reply);
				return 0;
			}
			
			restReply(fd, format, 400, "error", "bad request");
			return -1;
		}
		else
		{
			restReply(fd, format, 400, "error", "bad path");
			return 1;
		}
	}
	else if (verb == GET)
	{
		char tLevels[256];

		if ((components == 3) && (component[2] == "get"))
		{
			lmTraceGet(tLevels);
			restReply(fd, format, 200, "trace levels", tLevels);
			return 0;
		}

		restReply(fd, format, 400, "error", "bad path");
		return 1;
	}
	else if (verb == PUT)
	{
		if ((component[2] != "set") || (components != 4))
		{
			restReply(fd, format, 400, "error", "bad path");
			return 1;
		}

		const char* s = component[3].c_str();

		if (strspn(s, "0123456789-,") != strlen(s))
		{
			restReply(fd, format, 400, "error", "bad request");
			return -1;
		}

		char tLevels[256];
		char reply[256];

		if (component[2] == "set")
		{
			LM_T(LmtLogRequest, ("Setting trace levels '%s'", s));
			lmTraceSet(s);
		}
		else if (component[2] == "add")
		{
			LM_T(LmtLogRequest, ("Adding trace levels '%s'", s));
			lmTraceAdd(s);
		}
		else
		{
			restReply(fd, format, 400, "error", "bad path");
			return -1;
		}

		lmTraceGet(tLevels);
		snprintf(reply, sizeof(reply), "new trace levels: '%s'", tLevels);
		restReply(fd, format, 200, "trace", reply);
		return 0;
	}

	allow = "PUT,GET,DELETE";
	restReply(fd, format, 405, "error", "bad verb");
	return -1;
}



/* ****************************************************************************
*
* verboseRequest - set/clear verbose level
*
* DELETE /log/verbose
* PUT    /log/verbose/off
* PUT    /log/verbose/set/1
* PUT    /log/verbose/set/2
* PUT    /log/verbose/set/3
* PUT    /log/verbose/set/4
* PUT    /log/verbose/set/5
* GET    /log/verbose
* 
*/
static int verboseRequest(int fd, Verb verb, Format format, int components, std::string* component)
{
	LM_T(LmtLogRequest, ("verboseRequest"));

	if (verb == DELETE)
	{
		LM_T(LmtLogRequest, ("verboseRequest: DELETE"));

		if (components == 2)
		{
			lmVerbose  = false;
			lmVerbose2 = false;
			lmVerbose3 = false;
			lmVerbose4 = false;
			lmVerbose5 = false;

			restReply(fd, format, 200, "verbose", "verbose off");
			return 0;
		}

		restReply(fd, format, 400, "error", "bad path");
		return 1;
	}
	else if (verb == GET)
	{
		if (components == 2)
		{
			char verbose[5];
			int  verboseLevel = 0;

			LM_T(LmtLogRequest, ("verboseRequest: GET"));

			if (lmVerbose  == true)  verboseLevel = 1;
			if (lmVerbose2 == true)  verboseLevel = 2;
			if (lmVerbose3 == true)  verboseLevel = 3;
			if (lmVerbose4 == true)  verboseLevel = 4;
			if (lmVerbose5 == true)  verboseLevel = 5;

			snprintf(verbose, sizeof(verbose), "%d", verboseLevel);
			restReply(fd, format, 200, "verbose level", verbose);
			return 0;
		}

		restReply(fd, format, 400, "error", "bad path");
		return 1;
	}
	else if (verb == PUT)
	{
		LM_T(LmtLogRequest, ("verboseRequest: PUT"));

		if ((components == 3) && (component[2] == "off"))
		{
            lmVerbose  = false;
            lmVerbose2 = false;
            lmVerbose3 = false;
            lmVerbose4 = false;
            lmVerbose5 = false;

			restReply(fd, format, 200, "verbose", "verbose off");
			return 0;
		}
		else if ((components == 4) && (component[2] == "set"))
		{
			int level = atoi(component[3].c_str());

			if ((level == 0) && (component[3] == "0"))
			{
				lmVerbose  = false;
				lmVerbose2 = false;
				lmVerbose3 = false;
				lmVerbose4 = false;
				lmVerbose5 = false;

				restReply(fd, format, 200, "verbose", "verbose off");
				return 0;
			}
			else if (level <= 5)
			{
				if (level >= 1) lmVerbose  = true;
				if (level >= 2) lmVerbose2 = true;
				if (level >= 3) lmVerbose3 = true;
				if (level >= 4) lmVerbose4 = true;
				if (level >= 5) lmVerbose5 = true;

				LM_T(LmtLogRequest, ("Setting verbose level to %d", level));
				restReply(fd, format, 200, "verbose", "new level applied");
				return 0;
			}

			restReply(fd, format, 400, "error", "bad request");
			return 1;
		}
	}

	allow = "PUT,GET,DELETE";
	restReply(fd, format, 405, "error", "bad verb");

	return -1;
}



/* ****************************************************************************
*
* debugRequest - 
*
*  DELETE /log/debug
*  GET    /log/debug
*  PUT    /log/debug/on
*  PUT    /log/debug/off
*/
static int debugRequest(int fd, Verb verb, Format format, int components, std::string* component)
{
	if (verb == DELETE)
	{
		if (components == 2)
		{
			lmDebug = false;
			restReply(fd, format, 200, "debug", "debug off");
			return 0;
		}

		restReply(fd, format, 400, "error", "bad path");
		return 1;
	}
	else if (verb == GET)
	{
		const char* debug = (lmDebug == true)? "on" : "off";

		if (components == 2)
		{
			restReply(fd, format, 200, "debug", debug);
			return 0;
		}

		restReply(fd, format, 400, "error", "bad path");
		return 1;
	}
	else if (verb == PUT)
	{
		if (components == 3)
		{
			if (component[2] == "on")
			{
				lmDebug = true;
				restReply(fd, format, 200, "debug", "on");
				return 0;
			}
			else if (component[2] == "off")
            {
				lmDebug = false;
				restReply(fd, format, 200, "debug", "off");
				return 0;
			}

			restReply(fd, format, 400, "error", "bad path");
			return 1;
		}

		restReply(fd, format, 400, "error", "bad path");
		return 1;
	}

	allow = "PUT,GET,DELETE";
	restReply(fd, format, 405, "error", "bad verb");

	return -1;
}



/* ****************************************************************************
*
* readsRequest - 
*
*  DELETE /log/reads
*  GET    /log/reads
*  PUT    /log/reads/on
*  PUT    /log/reads/off
*/
static int readsRequest(int fd, Verb verb, Format format, int components, std::string* component)
{
	if (verb == DELETE)
	{
		if (components == 2)
		{
			lmReads = false;
			restReply(fd, format, 200, "reads", "reads off");
			return 0;
		}

		restReply(fd, format, 400, "error", "bad path");
		return 1;
	}
	else if (verb == GET)
	{
		const char* reads = (lmReads == true)? "on" : "off";

		if (components == 2)
		{
			restReply(fd, format, 200, "reads", reads);
			return 0;
		}

		restReply(fd, format, 400, "error", "bad path");
		return 1;
	}
	else if (verb == PUT)
	{
		if (components == 3)
		{
			if (component[2] == "on")
			{
				lmReads = true;
				restReply(fd, format, 200, "reads", "on");
				return 0;
			}
			else if (component[2] == "off")
            {
				lmReads = false;
				restReply(fd, format, 200, "reads", "off");
				return 0;
			}

			restReply(fd, format, 400, "error", "bad path");
			return 1;
		}

		restReply(fd, format, 400, "error", "bad path");
		return 1;
	}

	allow = "PUT,GET,DELETE";
	restReply(fd, format, 405, "error", "bad verb");

	return -1;
}



/* ****************************************************************************
*
* writesRequest - 
*
*  DELETE /log/writes
*  GET    /log/writes
*  PUT    /log/writes/on
*  PUT    /log/writes/off
*/
static int writesRequest(int fd, Verb verb, Format format, int components, std::string* component)
{
	if (verb == DELETE)
	{
		if (components == 2)
		{
			lmWrites = false;
			restReply(fd, format, 200, "writes", "writes off");
			return 0;
		}

		restReply(fd, format, 400, "error", "bad path");
		return 1;
	}
	else if (verb == GET)
	{
		const char* writes = (lmWrites == true)? "on" : "off";

		if (components == 2)
		{
			restReply(fd, format, 200, "writes", writes);
			return 0;
		}

		restReply(fd, format, 400, "error", "bad path");
		return 1;
	}
	else if (verb == PUT)
	{
		if (components == 3)
		{
			if (component[2] == "on")
			{
				lmWrites = true;
				restReply(fd, format, 200, "writes", "on");
				return 0;
			}
			else if (component[2] == "off")
            {
				lmWrites = false;
				restReply(fd, format, 200, "writes", "off");
				return 0;
			}

			restReply(fd, format, 400, "error", "bad path");
			return 1;
		}

		restReply(fd, format, 400, "error", "bad path");
		return 1;
	}

	allow = "PUT,GET,DELETE";
	restReply(fd, format, 405, "error", "bad verb");

	return -1;
}



/* ****************************************************************************
*
* logRequest - 
*/
int logRequest(int fd, Verb verb, Format format, int components, std::string* component)
{
	if (component[1] == "verbose")
		return verboseRequest(fd, verb, format, components, component);
	else if (component[1] == "debug")
		return debugRequest(fd, verb, format, components, component);
	else if (component[1] == "trace")
		return traceRequest(fd, verb, format, components, component);
	else if (component[1] == "reads")
		return readsRequest(fd, verb, format, components, component);
	else if (component[1] == "writes")
		return writesRequest(fd, verb, format, components, component);

	restReply(fd, format, 400, "error", "bad path");

	return -1;
}
