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
* OR: connects to tektronixTunnel to receive real data and injects that data in samson
*
*/
#include <signal.h>             // signal, SIGINT, ...
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
#include "parseArgs/paUsage.h"
#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"

#include "samson/client/SamsonClient.h"         // samson::SamsonClient
#include "samson/common/samsonVars.h"
#include "samson/client/SamsonPushBuffer.h"


/* ****************************************************************************
*
* Option variables
*/
SAMSON_ARG_VARS;

char            controller[256];
char            file[256];
char            queueName[256];
bool            tektronix;
bool            fake;
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

    { "-tektronix", &tektronix,    "SS_PLP_TEKTRONIX",      PaBool,    PaOpt,          false,    false,  true,  "act as tektronix consumer"  },
    { "-fake",      &fake,         "SS_PLP_FAKE",           PaBool,    PaOpt,          false,    false,  true,  "fake data"                  },
    { "-host",       host,         "SS_PLP_TEKTRONIX_HOST", PaString,  PaOpt,           PaND,    PaNL,   PaNL,  "tektronix tunnel host"      },
    { "-port",       &port,        "SS_PLP_TEKTRONIX_PORT", PaShortU,  PaOpt,           1099,    1024,  65535,  "tektronix tunnel port"      },
    { "-queue",      queueName,    "SS_PLP_QUEUE_NAME",     PaString,  PaOpt,           PaND,    PaNL,   PaNL,  "name of the queue"          },
    { "-timeout",    &timeout,     "SS_PLP_TIMEOUT",        PaInt,     PaOpt,              0,       0,   3600,  "timeout"                    },
    { "-rate",       &rate,        "SS_PLP_RATE",           PaDouble,  PaOpt,         _i 1.0,    PaNL,   PaNL,  "rate"                       },
    { "-node",       controller,   "",                      PaString,  PaOpt, _i "localhost",    PaNL,   PaNL,  "SAMSON node"                },
    { "-file",       file,         "SS_PLP_FILE",           PaString,  PaOpt, _i "generator",    PaNL,   PaNL,  "file"                       },

    PA_END_OF_ARGS
};



/* ****************************************************************************
*
* Global variables 
*/
unsigned int  bufSize = M(8) + 1;
char          buf[M(8) + 1];            // MAX size: 8 megas ... ?
char          buffer[M(8) + 1];         // MAX size: 8 megas ... ?
char          savedBuffer[128 * 1024];
int           savedMissing      = 0;
int           savedLen          = 0;
int           storageFd         = -1;
int           nbAccumulated     = 0;
int           packets           = 0;



/* ********************************************************************************
*
* Handler for SIGINT
*/
sig_atomic_t signaled_quit = false;
void captureSIGINT(int s)
{
    signaled_quit = true;
	lmVerbose     = false;
}


/* ****************************************************************************
*
* connectToServer - 
*/
int connectToServer(void)
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
			LM_W(("connect intent %d failed: %s", tri, strerror(errno)));
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
* bufPresent - 
*/
void bufPresent(const char* title, char* buf, int bufLen)
{
	int ix = 0;

	if (lmVerbose < 2)
	   return;

	printf("----- %s -----\n", title);

	while (ix < bufLen)
	{
		if (ix % 16 == 0)
			printf("%08x:  ", ix);
		printf("%02x ", buf[ix] & 0xFF);
		++ix;
		if (ix % 16 == 0)
			printf("\n");
	}

	printf("\n");
	printf("\n");	
}



/* ****************************************************************************
*
* bufPush - 
*/
void bufPush(char* buf, int size, samson::SamsonPushBuffer* pushBuffer)
{
    int    packetLen;
	char*  initialBuf = buf;
	int    totalLen   = 0;
	int    ppackets   = 0;

	LM_VVV(("Pushing a buffer of %d bytes", size));

    //
    // Any data leftover from last push?
    // Add the data from buf to make it a whole packet,
    // and then forward this 'incomplete' packet first
    //
    if (savedLen != 0)
    {
        LM_V(("Detected a saved buf of %d bytes, adding another %d bytes to it", savedLen, savedMissing));

        if (size >= savedMissing)
        {
            int* iP = (int*) savedBuffer;
            LM_D(("Restoring a saved packet: 0x%08x 0x%08x 0x%08x 0x%08x", iP[0], iP[1], iP[2], iP[3]));

            memcpy(&savedBuffer[savedLen], buf, savedMissing);

            LM_V(("Pushing a border record of %d bytes", savedLen + savedMissing));

            if (fake == false)
            {
			   LM_VVV(("Pushing buffer of %d bytes to samson", savedLen + savedMissing));
			   pushBuffer->push(savedBuffer, savedLen + savedMissing, true); // Is the '4' included here ... ?
			}
            else
                LM_V(("NOT pushing buffer of %d bytes (dataLen: %d)", savedLen + savedMissing, *iP));

            buf   = &buf[savedMissing];
            size    -= savedMissing;
            savedLen = 0;
            packets  = packets + 1;
        }
        else
        {
            LM_D(("Buffer too small to fill an entire packet - copying a part and reading again ..."));
            memcpy(&savedBuffer[savedLen], buf, size);
            savedLen += size;
            return;
        }
    }


    //
    // Now, finally, loop over all packets in the buf and write them to file
    //
    while (size > 0)
    {
        packetLen = ntohl(*((int*) buf));

        if ((packets % 100) == 0)
            LM_V(("parsed packet %d of %d data length (bigendian: 0x%x)", packets, packetLen, *((int*) buf)));

        // We are having problems when dying, so we'll try to avoid the LM_X,
        // and try to process the block
		if (packetLen > 3000) // For example ...
		{
			packetLen = ntohl(packetLen);
			if (packetLen > 3000)
				LM_X(1, ("Bad packetLen: %d (original: 0x%x, htohl: 0x%x)", packetLen, *((int*) buf), ntohl(*((int*) buf))));
		}

		LM_D(("packetLen: %d", packetLen));
        if (size >= packetLen + 4)
        {
			totalLen += packetLen + 4;

            ++packets;
            ++ppackets;
            LM_D(("Got package %d (grand total bytes read: %d)", packets, nbAccumulated));

			LM_VVV(("Don't push packet %d here ... push all packets afterwards ...", ppackets));
            // pushBuffer->push(buf, packetLen + 4, true);

            buf = &buf[packetLen + 4];
            size  -= (packetLen + 4);
        }
        else
        {
			if (totalLen != 0)
			{
                int* iP = (int*) initialBuf;

			    LM_VVV(("Pushing a block of %d packets (%d bytes)", ppackets, totalLen));

                if (fake == false)
				{
					pushBuffer->push(initialBuf, totalLen, true);
					totalLen = 0;
					ppackets = 0;
				}
                else
                    LM_V(("NOT pushing buffer of %d bytes (dataLen: %d)", totalLen, *iP));
			}
			else
				LM_W(("totalLen == 0 ..."));

            memcpy(savedBuffer, buf, size);
            savedLen       = size;
            savedMissing   = packetLen + 4 - savedLen;

            LM_D(("Saved packet %d of %d bytes (packetLen: %d, so %d bytes missing ...)", packets, savedLen, packetLen, savedMissing));

            int* iP = (int*) savedBuffer;
            LM_D(("saved: 0x%x 0x%x 0x%x 0x%x", iP[0], iP[1], iP[2], iP[3]));
            return; // read more ...
        }
    }

	if ((totalLen != 0) && (fake == false))
	{
		LM_VVV(("Pushing buffer of %d bytes to samson", totalLen));
		pushBuffer->push(initialBuf, totalLen, true);
		totalLen = 0;
	}
}



/* ****************************************************************************
*
* readFromServer - 
*/
void readFromServer(int fd, samson::SamsonPushBuffer* pushBuffer)
{
	int nb;

    LM_VVV(("Reading from server"));
	while (signaled_quit == false)
	{
        // Read as much as we possibly can ...
        nb = read(fd, buffer, sizeof(buffer));
		LM_VVV(("Read %d bytes from tunnel", nb));
        if (nb > 0)
        {
            nbAccumulated += nb;

            LM_D(("Read %d bytes of data from tunnel (grand total: %d)", nb, nbAccumulated));
			bufPresent("Read from Tunnel", buffer, nb);

			LM_VVV(("Calling bufPush with a buffer of %d bytes", nb));
            bufPush(buffer, nb, pushBuffer);
        }
        else
        {
            if (nb == -1)
                LM_E(("Error reading from the Tunnel: %s", strerror(errno)));
            else
                LM_E(("The Tunnel closed the connection?"));

            close(fd);
            fd       = connectToServer();
            savedLen = 0;
        }
    }
}



/* ****************************************************************************
*
* tektronixData - 
*/
void tektronixData(samson::SamsonPushBuffer* pushBuffer)
{
    int fd;

    // 1. connect to host:port ...
    LM_V(("connecting to %s:%d", host, port));
    fd = connectToServer();
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
               " size " << au::str(total_size , "bytes") << " time " << au::str_time(elapsed_seconds) <<
               " theoretical time " << au::str_time(theoretical_seconds)<<"\n";
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
    paConfig("builtin prefix",                (void*) "SS_PLP_");
    paConfig("usage and exit on any warning", (void*) true);
    paConfig("log to screen",                 (void*) "only errors");
    paConfig("log file line format",          (void*) "TYPE:DATE:EXEC/FILE[LINE] FUNC: TEXT");
    paConfig("screen line format",            (void*) "TYPE@TIME  EXEC: TEXT (FUNC)");
    paConfig("log to file",                   (void*) true);
    paConfig("exit on usage",                 (void*) false);

    paParse(paArgs, argC, (char**) argV, 1, false);

    if (signal(SIGINT, captureSIGINT) == SIG_ERR)
        LM_W(("SIGINT cannot be handled"));
    signaled_quit = false;

    // Instance of the client to connect to SAMSON system
    samson::SamsonClient client("push");

    // Set 1G RAM for uploading content
    client.setMemory(1024 * 1024 * 1024);

    LM_V(("Connecting to '%s'", controller));

    // Initialize connection
    samson::SamsonPushBuffer* pushBuffer;
    if ((tektronix == true) && (fake == true))
        pushBuffer = NULL;
    else
    {
        if (!client.init(controller))
            LM_X(0, ("Error connecting to samson cluster (controller at: '%s'): %s\n" , controller, client.getErrorMessage().c_str()));

        pushBuffer = new samson::SamsonPushBuffer(&client, queueName);
    }

    LM_V(("rate: %f MBs", rate));
    if (timeout > 0)
        LM_V(("timeout: %ds", timeout));

    if (tektronix == true)
        tektronixData(pushBuffer);
    else if (fake == true)
        fakeData(pushBuffer);
    else
    {
        fprintf(stderr, "%s: bad parameters - either '-tektronix' or '-fake' must be used", paProgName);
        paUsage(paArgs);
    }
    
    // Only here if we received a SIGQUIT
    // Last push
    pushBuffer->flush();

    LM_V(("--------------------------------------------------------------------------------"));
    LM_V(("PushBuffer info:   %s", pushBuffer->rate.str().c_str()));
    LM_V(("SamsonClient info: %s", client.rate.str().c_str()));
    LM_V(("--------------------------------------------------------------------------------"));


    // Wait until all operations are complete
    LM_V(("Waiting for all the push operations to complete..."));
    client.waitUntilFinish();
}
