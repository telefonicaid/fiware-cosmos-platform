/* ****************************************************************************
*
* FILE                     main_samsonSupervisor.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Dec 14 2010
*
*/
#include "baTerm.h"             // baTermSetup
#include "logMsg.h"             // LM_*
#include "parseArgs.h"          // parseArgs

#include "NetworkInterface.h"   // DataReceiverInterface
#include "iomMsgSend.h"         // iomMsgSend
#include "iomConnect.h"         // iomConnect
#include "Endpoint.h"           // Endpoint
#include "Network.h"            // Network
#include "Process.h"            // Process, processAdd, ...



/* ****************************************************************************
*
* Option variables
*/
int              endpoints;
char             cfPath[80];



#define CFP (long int) "/opt/samson/config/platformProcesses"
/* ****************************************************************************
*
* Parse arguments
*/
PaArgument paArgs[] =
{
	{ "-endpoints", &endpoints, "ENDPOINTS", PaInt, PaOpt,   20,    3,  100, "number of endpoints" },
	{ "-config",    &cfPath,    "CF_FILE",   PaStr, PaOpt,  CFP, PaNL, PaNL, "path to config file" },

	PA_END_OF_ARGS
};



/* ****************************************************************************
*
* logFd - file descriptor for log file used in all libraries
*/
int logFd = -1;



/* ****************************************************************************
*
* SamsonSupervisor - 
*/
class SamsonSupervisor : public ss::DataReceiverInterface
{
public:
	SamsonSupervisor(ss::Network* nwP) { networkP = nwP; }

	virtual int receive(int fromId, int nb, ss::Message::Header* headerP, void* dataP);

private:
	ss::Network*    networkP;
};



/* ****************************************************************************
*
* help - 
*/
static void help(void)
{
	printf("h - print this help message\n");
	printf("q - quit\n");
	printf("s - start samson\n");
}



/* ****************************************************************************
*
* start - 
*/
static void start(void)
{
	int                     ix = 0;
	int                     s;
	Process*                p;
	ss::Message::SpawnData  spawnData;

	while (1)
	{
		p = processGet(ix);

		if (p == NULL)
			return;

		strcpy(spawnData.name, p->name);
		spawnData.port    = p->port;
		spawnData.verbose = lmVerbose;
		spawnData.reads   = lmReads;
		spawnData.writes  = lmWrites;
		lmTraceGet(spawnData.traceV);
		/* args ... */

		LM_D(("starting process %d (%s in %s with port %d)", ix, p->name, p->host, p->port));
		if (strcmp(spawnData.name, "Controller"))
			s = iomMsgSend(p->spawner->fd, p->spawner->host, "samsonSupervisor", ss::Message::ControllerSpawn, ss::Message::Msg, &spawnData, sizeof(spawnData));
		else if (strcmp(spawnData.name, "Worker"))
			s = iomMsgSend(p->spawner->fd, p->spawner->host, "samsonSupervisor", ss::Message::WorkerSpawn, ss::Message::Msg, &spawnData, sizeof(spawnData));
		if (s != 0)
			LM_E(("iomMsgSend: error %d", s));
	}
}



/* ****************************************************************************
*
* connectToSpawners - 
*/
static void connectToSpawners(void)
{
	int       ix = 0;
	Process*  p;
	Spawner*  s;

	while (1)
	{
		int fd;

		p = processGet(ix++);
		if (p == NULL)
			return;
		
		if ((s = spawnerGet(p->host)) != NULL)
		{
			p->spawner = s;
			continue;
		}

		LM_D(("connecting to spawner in %s on port %d", p->host, 1233));
		fd = iomConnect(p->host, 1233);
		if (fd == -1)
			LM_X(1, ("error connecting to spawner in %d, port %d", p->host, 1233));

		s = spawnerAdd(p->host, 1233, fd);
		p->spawner = s;
	}
}



/* ****************************************************************************
*
* SamsonSupervisor::receive - 
*/
int SamsonSupervisor::receive(int fromId, int nb, ss::Message::Header* headerP, void* dataP)
{
	ss::Endpoint* ep = networkP->endpointLookup(fromId);

	if (ep == NULL)
		LM_RE(0, ("Cannot find endpoint with id %d", fromId));

	if (ep->type == ss::Endpoint::Fd)
	{
		char* msg = (char*) dataP;

		printf("\n");
		switch (*msg)
		{
		case 'h':
			help();
			break;

		case 'c':
			connectToSpawners();
			break;

		case 's':
			start();
			break;

		case 'q':
			LM_X(0, ("'q' pressed - I quit!"));

		default:
			LM_E(("Key '%c' has no function", *msg));
			help();
		}

		printf("\n");
		return 0;
	}

	switch (headerP->code)
	{
	case ss::Message::WorkerSpawn:
	case ss::Message::ControllerSpawn:

	default:
		LM_X(1, ("Don't know how to treat '%s' message", ss::Message::messageCode(headerP->code)));
	}

	return 0;
}



/* ****************************************************************************
*
* argsParse - 
*/
static void argsParse(char* line, char* host, char* process, char* port, char** args, int* argCount)
{
	int  ix;
	int  argIx = 0;
	

	LM_D(("=============================================================="));
	LM_D(("Parsing line: %s", line));

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


	/* 4. Get 'host' */
	ix = 0;
	while (line[ix] != 0)
	{
		if (line[ix] == ' ')
		{
			line[ix] = 0;
			strcpy(host, line);
			line = &line[ix + 1];
			LM_D(("Got host '%s'. line: '%s'", host, line));
			break;
		}
		ix += 1;
	}


	/* 5. Get 'process' */
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
			LM_D(("Got process '%s'. line: '%s'", process, line));
            break;
        }
        ix += 1;
    }

	/* 6. Get 'port' */
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
            strcpy(port, line);
            line = &line[ix + 1];
			LM_D(("Got port '%s'. line: '%s'", port, line));
            break;
        }
        ix += 1;
    }


	LM_D(("Parsing args, line: '%s'", line));
	/* 7. Get rest of args */
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

		LM_D(("Got arg %d: '%s'. line: '%s'", argIx, start, line));
		args[argIx++] = strdup(start);
		LM_D(("REST: '%s'", line));
	}

	*argCount = argIx;
}



/* ****************************************************************************
*
* cfParse - 
*/
static void cfParse(char* cfPath)
{
	char    line[80];
	int     lineNo = 0;
	FILE*   fP;

	fP = fopen(cfPath, "r");
	if (fP == NULL)
		LM_X(1, ("opening '%s': %s", cfPath, strerror(errno)));

	while (fgets(line, sizeof(line), fP) != NULL)
	{
		char  host[64];
		char  process[64];
		int   port;

		++lineNo;
		sscanf(line, "%s%s%d", host, process, &port);
		if ((strcmp(process, "Controller") != 0) && (strcmp(process, "Worker") != 0))
			LM_X(1, ("%s[%d]: parse error", cfPath, lineNo));
		LM_D(("read line: %s", line));
	}

	rewind(fP);	
	while (fgets(line, sizeof(line), fP) != NULL)
	{
		char   host[32];
		char   process[32];
		char   port[32];
		char*  args[100];
		int    argCount;

		memset(args, 0, sizeof(args));
		LM_D(("parsing line: %s", line));
		argsParse(line, host, process, port, args, &argCount);
		LM_D(("Adding process '%s' in %s (port %s) with %d args", process, host, port, argCount));
		processAdd(host, process, atoi(port), args, argCount);
	}
}



/* ****************************************************************************
*
* cfPresent - 
*/
static void cfPresent(void)
{
	Process* p;
	int      ix = 0;

	while (1)
	{
		int aIx;

		p = processGet(ix);
		if (p == NULL)
			break;

		printf("Process %d:\n", ix);
		printf("  Name:     %s\n", p->name);
		printf("  Host:     %s\n", p->host);
		printf("  Port:     %d\n", p->port);
		printf("  Args:     %d\n", p->argCount);

		for (aIx = 0; aIx < p->argCount; aIx++)
			printf("    Arg %02d:  %s\n", aIx, p->arg[aIx]);

		printf("\n");
		++ix;
	}
}


/* ****************************************************************************
*
* main - 
*/
int main(int argC, const char *argV[])
{
	ss::Network*           networkP;
	SamsonSupervisor*      supervisorP;

	paConfig("prefix",                        (void*) "SSS_");
	paConfig("usage and exit on any warning", (void*) true);
	paConfig("log to screen",                 (void*) "only errors");
	paConfig("log file line format",          (void*) "TYPE:DATE:EXEC-AUX/FILE[LINE] FUNC: TEXT");
	paConfig("screen line format",            (void*) "TYPE: TEXT");
	paConfig("log to file",                   (void*) true);

	paParse(paArgs, argC, (char**) argV, 1, false);

	LM_M(("parsing input file '%s'", cfPath));
	cfParse(cfPath);
	cfPresent();
	LM_X(0, ("startup file parsing done"));

	networkP = new ss::Network(endpoints, 0);

	networkP->init(ss::Endpoint::Supervisor, NULL, 0, NULL);
	supervisorP = new SamsonSupervisor(networkP);

	networkP->setDataReceiver(supervisorP);

	baTermSetup();
	networkP->fdSet(0, "stdin", "stdin");
	networkP->run();

	return 0;
}
