/* ****************************************************************************
*
* FILE                     passiveLocationPush - example app using samsonClient lib
*
* AUTHOR                   Andreu Urruela, Ken Zangelin
*
* LAST MODIFICATION DATE   Jan 10 2012
*
*
* Generate random xml documents simulating information from OSS Passive Location pilot
* OR: connects to arcanumTunnel to receive real data and injects that data in samson
*
*/
#include <stdio.h>              // printf
#include <stdlib.h>             // exit
#include <string.h>             // memcpy
#include <iostream>             // std::cout
#include <time.h>               // strptime, struct tm
#include <sys/types.h>          // types needed by socket include files
#include <stdlib.h>             // free
#include <sys/socket.h>         // socket, bind, listen
#include <sys/un.h>             // sockaddr_un
#include <netinet/in.h>         // struct sockaddr_in
#include <netdb.h>              // gethostbyname
#include <arpa/inet.h>          // inet_ntoa
#include <netinet/tcp.h>        // TCP_NODELAY
#include <unistd.h>             // close
#include <fcntl.h>              // fcntl, F_SETFD

#include "au/time.h"            // au::todatString()
#include "au/string.h"          // au::str()
#include "au/Cronometer.h"      // au::Cronometer
#include "au/CommandLine.h"     // au::CommandLine

#include "parseArgs/parseArgs.h"
#include "parseArgs/paConfig.h"
#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"

#include "samson/client/SamsonClient.h"         // samson::SamsonClient
#include "samson/common/samsonVars.h"



/* ****************************************************************************
*
* Option variables
*/
SAMSON_ARG_VARS;

char            controller[256];
char            file[256];
char            queueName[256];
bool            arcanum;
char            host[256];
unsigned short  port       = 0;
int             timeout    = 0;
double          rate       = 1;


#define K(k) (1024 * k)
#define M(m) (1024 * 1024 * m)
/* ****************************************************************************
*
* parse arguments
*/
PaArgument paArgs[] =
{
    SAMSON_ARGS,

    { "-arcanum",    &arcanum,     "SS_PLP_ARCANUM",      PaBool,    PaOpt,          false,    false,  true,  "act as arcanum consumer"  },
    { "-host",       host,         "SS_PLP_ARCANUM_HOST", PaString,  PaOpt,           PaND,    PaNL,   PaNL,  "arcanum tunnel host"      },
    { "-port",       &port,        "SS_PLP_ARCANUM_PORT", PaShortU,  PaOpt,           1099,    1024,  65535,  "arcanum tunnel port"      },
    { "-queue",      queueName,    "SS_PLP_QUEUE_NAME",   PaString,  PaOpt,           PaND,    PaNL,   PaNL,  "name of the queue"        },
    { "-timeout",    &timeout,     "SS_PLP_TIMEOUT",      PaInt,     PaOpt,              0,       0,   3600,  "timeout"                  },
    { "-rate",       &rate,        "SS_PLP_RATE",         PaDouble,  PaOpt,         _i 1.0,    PaNL,   PaNL,  "rate"                     },
    { "-controller", controller,   "SS_PLP_CONTROLLER",   PaString,  PaOpt, _i "localhost",    PaNL,   PaNL,  "controller IP"            },
    { "-file",       file,         "SS_PLP_FILE",         PaString,  PaOpt, _i "generator",    PaNL,   PaNL,  "file"                     },

    PA_END_OF_ARGS
};



/* ****************************************************************************
*
* Global variables 
*/
unsigned int  bufSize = M(8) + 1;
char          buf[M(8) + 1];   // MAX size: 8 megas ... ?



/* ****************************************************************************
*
* connectToServer - 
*/
int connectToServer(const char* host, unsigned short port)
{
	struct hostent*     hp;
	struct sockaddr_in  peer;
	int                 fd;

	if (host == NULL)
		LM_RE(-1, ("no hostname given"));
	if (port == 0)
		LM_RE(-1, ("Cannot connect to '%s' - port is ZERO", host));

	if ((hp = gethostbyname(host)) == NULL)
		LM_RE(-1, ("gethostbyname(%s): %s", host, strerror(errno)));

	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		LM_RE(-1, ("socket: %s", strerror(errno)));
	
	memset((char*) &peer, 0, sizeof(peer));

	peer.sin_family      = AF_INET;
	peer.sin_addr.s_addr = ((struct in_addr*) (hp->h_addr))->s_addr;
	peer.sin_port        = htons(port);

	LM_V(("Connecting to %s:%d", host, port));
	int retries = 200;
	int tri     = 0;

	while (1)
	{
		if (connect(fd, (struct sockaddr*) &peer, sizeof(peer)) == -1)
		{
			++tri;
			// LM_E(("connect intent %d failed: %s", tri, strerror(errno)));
			usleep(500000);
			if (tri > retries)
			{
				close(fd);
				LM_RE(-1, ("Cannot connect to %s, port %d (even after %d retries)", host, port, retries));
			}
		}
		else
			break;
	}

	return fd;
}



/* ****************************************************************************
*
* readFromServer - 
*/
void readFromServer(int fd, samson::SamsonPushBuffer* pushBuffer)
{
	int           nb;
	unsigned int  tot;
    unsigned int  dataLen;

	while (1)
	{
        //
        // Read header - 4 bytes that contains the length of the nextcoming data
        //
        nb = read(fd, &dataLen, sizeof(dataLen));
		if (nb == -1)
			LM_X(1, ("error reading from socket: %s", strerror(errno)));
		else if (nb == 0)
		{
            LM_E(("Reading header: got zero bytes ... closing connection and reconnecting!"));
            close(fd);
            fd = connectToServer(host, port);
            continue;
        }

        if (dataLen > bufSize)
            LM_X(1, ("next data package too big ... (%d bytes) - change bufSize and recompile!", dataLen));

        LM_M(("Got a header. dataLen: %d. Now reading the data ...", dataLen));
		memset(buf, 0, bufSize + 1);


        //
        // Read data buffer
        //
        tot = 0;
        while (tot < dataLen)
        {
            nb = read(fd, &buf[tot], dataLen - tot);
            if (nb <= 0)
            {
                if (nb == -1)
                    LM_E(("error reading from socket: %s", strerror(errno)));
                else
                    LM_E(("Read zero bytes ... closing connection and reconnecting!"));
                    
                close(fd);
                fd = connectToServer(host, port);
                tot = 0;
                break;
            }

            tot += nb;
            LM_M(("Read %d of %d bytes of data", tot, dataLen));
        }

        if (tot == dataLen)
        {
            LM_M(("Read a buffer of %d bytes from %s - injecting it to samson", dataLen, host));
            pushBuffer->push(buf, dataLen, true);
        }
    }
}


/* ****************************************************************************
*
* arcanumData - 
*/
void arcanumData(samson::SamsonPushBuffer* pushBuffer)
{
    int fd;

    // 1. connect to host:port ...
    LM_V(("connecting to %s:%d", host, port));
    fd = connectToServer(host, port);
    if (fd == -1)
        LM_X(1, ("Error connecting to server '%s', port %d", host, port));

    LM_V(("calling readFromServer"));
    readFromServer(fd, pushBuffer);
}



/* ****************************************************************************
*
* fakeData - 
*/
void fakeData(samson::SamsonPushBuffer* pushBuffer)
{
    struct tm          tm;
    char               line[2048];
    const char*        lineP = line;
    au::Cronometer     cronometer;
    unsigned long int  total_size      = 0;
    unsigned long int  num_messages    = 0;
    FILE*              fp              = NULL;
    time_t             timeStamp       = 0;
    time_t             firstTimeStamp  = 0;
    time_t             offset_secs     = 0;

    if (strcmp(file, "generator") != 0)
    {
        if ((fp = fopen(file, "r")) == NULL)
            LM_X(1, ("Error opening file '%s' (errno:%d): %s", file, errno, strerror(errno)));
    }

    while (true)
    {
        char* pTime;
        char* p_sep;

        if (fp == NULL)
        {

            unsigned long int  used_id = rand() % 40000000;
            int                cell    = rand() % 20000;

            //snprintf(line, 1024 , "<message> <id> %lu </id> <cell> %d </cell> <time> %s </time>\n", used_id , cell , au::todayString().c_str() );
            snprintf(line, sizeof(line) , "<?xml version=\"1.0\" encoding=\"UTF-8\"?><ns0:AMRReport xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'  xmlns:ns0='http://O2.arcanum.vitria.com'  xsi:schemaLocation='http://O2.arcanum.vitria.com AMR.xsd'>  <SubscriberReport>    <User>      <IMSI>%lu</IMSI>      <IMEI>3563190407579307</IMEI>    </User>    <Authentication>      <Location>        <LocationArea>12115</LocationArea>        <CellID>%d</CellID>        <RoutingArea>134</RoutingArea>      </Location>    </Authentication>  </SubscriberReport>  <Timestamp>%s</Timestamp></ns0:AMRReport>\n", used_id , cell , au::todayString().c_str() );
        }
        else
        {
            if (fgets(line, sizeof(line), fp) == NULL)
            {
                rewind(fp);
                if (firstTimeStamp != 0)
                {
                    offset_secs += (timeStamp - firstTimeStamp);
                }
                continue;
            }
#define STR_TIMESTAMP_BEGIN "<Timestamp>"
            if ((pTime = strstr(line, STR_TIMESTAMP_BEGIN)) != NULL)
            {
                pTime += strlen(STR_TIMESTAMP_BEGIN);
                if ((p_sep = strchr(pTime, '<')) != NULL)
                {
                    *p_sep = '\0';
                    strptime(pTime, "%Y-%m-%dT%H:%M:%S", &tm);
                    timeStamp = mktime(&tm);
                    if (offset_secs != 0)
                    {
                        timeStamp += offset_secs;
                        localtime_r(&timeStamp, &tm);
                        strftime(pTime, 20, "%Y-%m-%dT%H:%M:%S", &tm);
                    }
                    else if (firstTimeStamp == 0)
                    {
                        firstTimeStamp = timeStamp;
                    }
                    *p_sep = '<';
                }
            }
        }

        size_t line_length  = strlen(line) + 1;
        total_size += line_length;
        num_messages++;

        pushBuffer->push(lineP, line_length, true);

        // Detect if we need to sleep....
        int theoretical_seconds  = ((double) total_size / ((double) (1024*1024) * (double) rate));
        int elapsed_seconds      = cronometer.diffTimeInSeconds();

        // Sleep some time to simulate a particular rate
        if (elapsed_seconds < theoretical_seconds)
        {
            int sleep_seconds = theoretical_seconds - elapsed_seconds;
            std::cerr << "Sleeping " << sleep_seconds << " seconds... num messages " << au::str(num_messages) << 
               " size " << au::str(total_size , "bytes") << " time " << au::time_string(elapsed_seconds) <<
               " theoretical time " << au::time_string(theoretical_seconds)<<"\n";
            sleep(sleep_seconds);
        }
    }
}



/* ****************************************************************************
*
* main - 
*/
int main(int argC, const char* argV[])
{
#if 0
    au::CommandLine cmd;

    cmd.set_flag_int("timeOut" , 0);         // 0 value for timeOut means no timeout
    cmd.set_flag_double("rate" , 1.0);
    cmd.set_flag_string("controller", "localhost");
    cmd.set_flag_string("file", "generator");

    cmd.parse(argc, argv);

    if (cmd.get_num_arguments() < 2)
    {
        fprintf(stderr,"Usage %s queue [ -controller controller_ip]  [ -rate rate_in_megabytes_per_second ]  [ -timeOut timeout_in_seconds ] [-file input_file]\n" , argv[0]);
        exit(0);
    }


    std::string queue_name = cmd.get_argument(1);
    std::string controller = cmd.get_flag_string("controller");
    std::string file = cmd.get_flag_string("file");

    int timeOut = cmd.get_flag_int("timeOut");
    double rate = cmd.get_flag_double("rate");
#endif


    paConfig("builtin prefix",                (void*) "SS_PLP_");
    paConfig("usage and exit on any warning", (void*) true);
    paConfig("log to screen",                 (void*) "only errors");
    paConfig("log file line format",          (void*) "TYPE:DATE:EXEC/FILE[LINE] FUNC: TEXT");
    paConfig("screen line format",            (void*) "TYPE@TIME  EXEC: TEXT (FUNC)");
    paConfig("log to file",                   (void*) true);
    paConfig("exit on usage",                 (void*) false);

    paParse(paArgs, argC, (char**) argV, 1, false);

    LM_M(("Here"));
    // Instance of the client to connect to SAMSON system
    samson::SamsonClient client;

    LM_M(("Here"));
    // Set 1G RAM for uploading content
    client.setMemory(1024 * 1024 * 1024);
    LM_M(("Here"));

    LM_V(("Connecting to '%s'", controller));

    // Initialize connection
    LM_M(("Here"));
    if (!client.init(controller))
        LM_X(0, ("Error connecting to samson cluster (controller at: '%s'): %s\n" , controller, client.getErrorMessage().c_str()));

    LM_M(("Here"));
    samson::SamsonPushBuffer* pushBuffer = new samson::SamsonPushBuffer(&client, queueName);
    LM_M(("Here"));

    LM_V(("rate: %f MBs", rate));
    if (timeout > 0)
        LM_V(("timeout: %ds", timeout));

    LM_M(("Here"));
    if (arcanum == true)
        arcanumData(pushBuffer);
    else
        fakeData(pushBuffer);
}
