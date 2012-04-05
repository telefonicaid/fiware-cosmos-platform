/* ****************************************************************************
*
* FILE                     main_samsonWorker.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Dec 14 2010
*
*/
#include <sys/socket.h>         // socket, bind, listen
#include <sys/un.h>             // sockaddr_un
#include <netinet/in.h>         // struct sockaddr_in
#include <netdb.h>              // gethostbyname
#include <arpa/inet.h>          // inet_ntoa
#include <netinet/tcp.h>        // TCP_NODELAY
#include <signal.h>

#include "parseArgs/parseArgs.h"
#include "parseArgs/paBuiltin.h"
#include "parseArgs/paIsSet.h"
#include "parseArgs/paConfig.h"
#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"

#include "au/LockDebugger.h"            // au::LockDebugger
#include "au/ThreadManager.h"

#include "engine/MemoryManager.h"
#include "engine/Engine.h"
#include "engine/DiskManager.h"
#include "engine/ProcessManager.h"

#include "samson/common/samsonVersion.h"
#include "samson/common/samsonVars.h"
#include "samson/common/SamsonSetup.h"
#include "samson/common/daemonize.h"
#include "samson/common/MemoryCheck.h"
#include "samson/network/WorkerNetwork.h"
#include "samson/worker/SamsonWorker.h"
#include "samson/isolated/SharedMemoryManager.h"
#include "samson/stream/BlockManager.h"
#include "samson/module/ModulesManager.h"



/* ****************************************************************************
*
* Option variables
*/
SAMSON_ARG_VARS;

bool            fg;
bool            monit;
bool            noLog;
int             valgrind;
int             port;
int             web_port;
char            lsHost[64];
unsigned short  lsPort;



/* ****************************************************************************
*
* parse arguments
*/
PaArgument paArgs[] =
{
    SAMSON_ARGS,
    { "-fg",        &fg,        "SAMSON_WORKER_FOREGROUND", PaBool,   PaOpt, false,                  false,  true,  "don't start as daemon"             },
    { "-monit",     &monit,     "SAMSON_WORKER_MONIT",      PaBool,   PaOpt, false,                  false,  true,  "to use with monit"                 },
    { "-port",      &port,      "",                         PaInt,    PaOpt, SAMSON_WORKER_PORT,     1,      9999,  "Port to receive new connections"   },
    { "-web_port",  &web_port,  "",                         PaInt,    PaOpt, SAMSON_WORKER_WEB_PORT, 1,      9999,  "Port to receive web connections"   },
    { "-nolog",     &noLog,     "SAMSON_WORKER_NO_LOG",     PaBool,   PaOpt, false,                  false,  true,  "no logging"                        },
    { "-valgrind",  &valgrind,  "SAMSON_WORKER_VALGRIND",   PaInt,    PaOpt, 0,                      0,        20,  "help valgrind debug process"       },
    { "-lsHost",    &lsHost,    "SAMSON_WORKER_LS_HOST",    PaString, PaOpt, _i "localhost",         PaNL,   PaNL,  "Host for Log Server"               },
    { "-lsPort",    &lsPort,    "SAMSON_WORKER_LS_PORT",    PaUShort, PaOpt, 2999,                   1,      9999,  "Port for Log Server"               },

    PA_END_OF_ARGS
};



/* ****************************************************************************
*
* global variables
*/
int                           logFd             = -1;
samson::WorkerNetwork*        networkP          = NULL;
samson::SamsonWorker*         worker            = NULL;
au::LockDebugger*             lockDebugger      = NULL;
engine::SharedMemoryManager*  smManager         = NULL;



/* ****************************************************************************
*
* man texts -
*/
static const char* manSynopsis         = " [OPTION]";
static const char* manShortDescription = "samsond is the main process in a SAMSON system.\n\n";
static const char* manDescription      =
    "\n"
    "samsond is the main process in a SAMSON system. All the nodes in the cluster has its own samsonWorker process\n"
    "All samsond processes are responsible for processing a segment of available data"
    "All clients of the platform ( delila's ) are connected to all samsonWorkers in the system"
    "See samson documentation to get more information about how to get a SAMSON system up and running"
    "\n";

static const char* manExitStatus    = "0      if OK\n 1-255  error\n";
static const char* manAuthor        = "Written by Andreu Urruela, Ken Zangelin and J.Gregorio Escalada.";
static const char* manReportingBugs = "bugs to samson-dev@tid.es\n";
static const char* manCopyright     = "Copyright (C) 2011 Telefonica Investigacion y Desarrollo";
static const char* manVersion       = SAMSON_VERSION;


void captureSIGINT( int s )
{
    s = 3;
    LM_X(1, ("Signal SIGINT"));
}

void captureSIGPIPE( int s )
{
    s = 3;
    LM_M(("Captured SIGPIPE"));
}

void captureSIGTERM( int s )
{
    s = 3;
    LM_M(("Captured SIGTERM"));

    LM_M(("Cleaning up"));
    std::string pid_file_name = au::str("%s/samsond.pid" , paLogDir );
    if ( remove (pid_file_name.c_str()) != 0)
    {
        LM_W(("Error deleting the pid file %s", pid_file_name.c_str() ));
    }
    exit(1);
}



void cleanup(void)
{
    
    // Stop all threads to clean up
    engine::Engine::stop();
    engine::DiskManager::stop();
    engine::ProcessManager::stop();
    if( networkP )
        networkP->stop();
    
    // Wait all threads to finsih
    au::ThreadManager::shared()->wait();
    
    google::protobuf::ShutdownProtobufLibrary();

    // Delete worker network
    LM_T(LmtCleanup, ("Clean up network"));
    if( networkP ) 
        delete networkP;
    
    LM_T(LmtCleanup, ("Shutting down worker components (worker at %p)", worker));
    if (worker != NULL)
    {
        LM_T(LmtCleanup, ("deleting worker"));
        delete worker;
        worker = NULL;
    }
    
    LM_T(LmtCleanup, ("Shutting down LockDebugger"));
    au::LockDebugger::destroy();

    if (smManager != NULL)
    {
        LM_T(LmtCleanup, ("Shutting down Shared Memory Manager"));
        delete smManager;
        smManager = NULL;
    }
    
    LM_T(LmtCleanup, ("destroying BlockManager"));
    samson::stream::BlockManager::destroy();

    LM_T(LmtCleanup, ("destroying ModulesManager"));
    samson::ModulesManager::destroy();

    LM_T(LmtCleanup, ("destroying ProcessManager"));
    engine::ProcessManager::destroy();

    LM_T(LmtCleanup, ("destroying DiskManager"));
    engine::DiskManager::destroy();

    LM_T(LmtCleanup, ("destroying MemoryManager"));
    engine::MemoryManager::destroy();

    LM_T(LmtCleanup, ("destroying Engine"));
    engine::Engine::destroy();

    LM_T(LmtCleanup, ("Shutting down SamsonSetup"));
    samson::SamsonSetup::destroy();
    
    LM_T(LmtCleanup, ("Calling paConfigCleanup"));
    paConfigCleanup();
    LM_T(LmtCleanup, ("Calling lmCleanProgName"));
    lmCleanProgName();
    LM_T(LmtCleanup, ("Cleanup DONE"));
}


static void valgrindExit(int v)
{
    if (v == valgrind)
    {
        LM_M(("Valgrind option is %d - I exit", v));
        exit(0);
    }
}



/* ****************************************************************************
*
* serverConnect - 
*/
int serverConnect(const char* host, unsigned short port)
{
    struct hostent*     hp;
    struct sockaddr_in  peer;
    int                 fd;

    if (port == 0)
        return -1;

    if ((hp = gethostbyname(host)) == NULL)
        return -1;

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        return -1;
    
    memset((char*) &peer, 0, sizeof(peer));

    peer.sin_family      = AF_INET;
    peer.sin_addr.s_addr = ((struct in_addr*) (hp->h_addr))->s_addr;
    peer.sin_port        = htons(port);

    int retries = 7200;
    int tri     = 0;

    while (1)
    {
        if (connect(fd, (struct sockaddr*) &peer, sizeof(peer)) == -1)
        {
            ++tri;
            usleep(500000);
            if (tri > retries)
            {
                close(fd);
                return -1;
            }
        }
        else
            break;
    }

    return fd;
}



/* ****************************************************************************
*
* logToLogServer - 
*/
int  lsFd = -1;

#if 0
#define P(s)        \
do                  \
{                   \
    printf s;       \
    printf("\n");   \
} while (0)
#else
#define P(s)
#endif


void logToLogServer(void* vP, char* text, char type, time_t secondsNow, int timezone, int dst, const char* file, int lineNo, const char* fName, int tLev, const char* stre)
{
    LogHeader  header;
    LogData    data;
    char       strings[1024];

    if (lsFd == -1)
    {
        P(("Connecting to LogServer at %s:%d", lsHost, lsPort));
        lsFd = serverConnect(lsHost, lsPort);
    }

    if (lsFd != -1)
    {
        int sIx = 0;

        P(("progName:    '%s' to strings[%d]", progName, sIx));
        strcpy(&strings[sIx], progName);
        sIx += strlen(progName);
        strings[sIx] = 0;
        ++sIx;

        P(("text:        '%s' to strings[%d]", text, sIx));
        strcpy(&strings[sIx], text);
        sIx += strlen(text);
        strings[sIx] = 0;
        ++sIx;

        P(("file:        '%s' to strings[%d]", file, sIx));
        strcpy(&strings[sIx], file);
        sIx += strlen(file);
        strings[sIx] = 0;
        ++sIx;

        P(("fName:       '%s' to strings[%d]", fName, sIx));
        strcpy(&strings[sIx], fName);
        sIx += strlen(fName);
        strings[sIx] = 0;
        ++sIx;

        if (stre != NULL)
        {
            P(("stre:        '%s' to strings[%d]", stre, sIx));
            strcpy(&strings[sIx], stre);
            sIx += strlen(stre);
            strings[sIx] = 0;
            ++sIx;
        }

        P(("------------------------------------------"));
        header.magic     = LM_MAGIC;
        header.dataLen   = sIx + sizeof(data);
        P(("magic:       %d", header.magic));
        P(("dataLen:     %d", header.dataLen));
        P(("------------------------------------------"));
        data.lineNo      = lineNo;
        data.traceLevel  = tLev;
        data.type        = type;
        data.unixSeconds = secondsNow;
        data.timezone    = timezone;
        data.dst         = dst;

        P(("lineNo:      %d",  data.lineNo));
        P(("traceLevel:  %d",  data.traceLevel));
        P(("type:        %d",  data.type));
        P(("unixSeconds: %ld", data.unixSeconds));
        P(("timezone:    %d",  data.timezone));
        P(("dst:         %d",  data.dst));
        P(("------------------------------------------"));

        struct iovec iov[3];

        iov[0].iov_base = &header;
        iov[0].iov_len  = sizeof(header);
        iov[1].iov_base = &data;
        iov[1].iov_len  = sizeof(data);
        iov[2].iov_base = strings;
        iov[2].iov_len  = sIx;
        
        ssize_t nb;
        ssize_t sz = iov[0].iov_len + iov[1].iov_len + iov[2].iov_len;

        P(("Writing a LogMsg of %ld bytes (dataLen: %d)", sz, header.dataLen));
        nb = writev(lsFd, iov, 3);
        if (nb != sz)
            P(("Written %ld bytes - not %ld!!!", nb, sz));
    }
}



/* ****************************************************************************
*
* main - 
*/
int main(int argC, const char *argV[])
{
    paConfig("builtin prefix",                (void*) "SS_WORKER_");
    paConfig("usage and exit on any warning", (void*) true);

    // Andreu: samsonWorker is not a console in foreground (debug) mode ( to ask to status with commands )
    paConfig("log to screen",                 (void*) "only errors");
    //paConfig("log to screen",                 (void*) (void*) false);
    
    paConfig("log file line format",          (void*) "TYPE:DATE:EXEC-AUX/FILE[LINE](p.PID)(t.TID) FUNC: TEXT");
    paConfig("screen line format",            (void*) "TYPE@TIME  EXEC: TEXT");
    paConfig("log to file",                   (void*) true);
    paConfig("log dir",                       (void*) "/var/log/samson/");

    paConfig("man synopsis",                  (void*) manSynopsis);
    paConfig("man shortdescription",          (void*) manShortDescription);
    paConfig("man description",               (void*) manDescription);
    paConfig("man exitstatus",                (void*) manExitStatus);
    paConfig("man author",                    (void*) manAuthor);
    paConfig("man reportingbugs",             (void*) manReportingBugs);
    paConfig("man copyright",                 (void*) manCopyright);
    paConfig("man version",                   (void*) manVersion);

    const char* extra = paIsSetSoGet(argC, (char**) argV, "-port");
    paParse(paArgs, argC, (char**) argV, 1, false, extra);

    lmOutHookSet(logToLogServer, NULL);
    // LM_T(19, ("LogServer test"));
    // exit(1);

    // Only add in foreground to avoid warning / error messages at the stdout
    if (fg)
        atexit(cleanup);

    valgrindExit(1);

    lmAux((char*) "father");

    LM_V(("Started with arguments:"));
    for (int ix = 0; ix < argC; ix++)
        LM_V(("  %02d: '%s'", ix, argV[ix]));

    logFd = lmFirstDiskFileDescriptor();
    
    // Capturing SIGPIPE
    if (signal(SIGPIPE, captureSIGPIPE) == SIG_ERR)
        LM_W(("SIGPIPE cannot be handled"));

    if (signal(SIGINT, captureSIGINT) == SIG_ERR)
        LM_W(("SIGINT cannot be handled"));

    if (signal(SIGTERM, captureSIGTERM) == SIG_ERR)
        LM_W(("SIGTERM cannot be handled"));

    // Init basic setup stuff (necessary for memory check)
    lockDebugger = au::LockDebugger::shared();  // VALGRIND complains ...
    samson::SamsonSetup::init(samsonHome, samsonWorking);          // Load setup and create default directories
    
    valgrindExit(2);

    // Check to see if the current memory configuration is ok or not
    if (samson::MemoryCheck() == false)
        LM_X(1,("Insufficient memory configured. Check %ssamsonWorkerLog for more information.", paLogDir));
    
    if ((fg == false) && (monit == false))
    {
        daemonize();
    }

    valgrindExit(3);
    // ------------------------------------------------------    
    // Write pid in /var/log/samson/samsond.pid
    // ------------------------------------------------------

    char pid_file_name[256];
    snprintf(pid_file_name, sizeof(pid_file_name), "%s/samsond.pid", paLogDir);
    FILE *file = fopen(pid_file_name, "w");
    if (!file)
        LM_X(1, ("Error opening file '%s' to store pid", pid_file_name));
    int pid = (int) getpid();
    if (fprintf(file, "%d" , pid) == 0)
       LM_X(1,("Error writing pid %d to file %s", pid, pid_file_name));
    fclose(file);
    // ------------------------------------------------------        
    

    if (monit)
    {
        LM_M(("monit test - I stay here ..."));
        while (1)
            sleep(1);
    }

    // Make sure this singleton is created just once
    LM_D(("createWorkingDirectories"));
    samson::SamsonSetup::shared()->createWorkingDirectories();      // Create working directories
    
    valgrindExit(4);
    LM_D(("engine::Engine::init"));
    engine::Engine::init();

    valgrindExit(5);
    LM_D(("engine::SharedMemoryManager::init"));
    engine::SharedMemoryManager::init( samson::SamsonSetup::shared()->getInt("general.num_processess") 
                                      ,samson::SamsonSetup::shared()->getUInt64("general.shared_memory_size_per_buffer")
                                      ); // VALGRIND complains ...
    smManager = engine::SharedMemoryManager::shared();

    valgrindExit(6);
    LM_D(("engine::DiskManager::init"));
    engine::DiskManager::init(1);

    valgrindExit(7);
    LM_D(("engine::ProcessManager::init"));
    engine::ProcessManager::init(samson::SamsonSetup::shared()->getInt("general.num_processess"));

    valgrindExit(8);
    LM_D(("engine::MemoryManager::init"));
    engine::MemoryManager::init(samson::SamsonSetup::shared()->getUInt64("general.memory"));

    valgrindExit(9);
    LM_D(("samson::ModulesManager::init"));
    samson::ModulesManager::init();

    valgrindExit(10);
    LM_D(("samson::stream::BlockManager::init"));
    samson::stream::BlockManager::init();

    valgrindExit(11);

    // Instance of network object and initialization
    // --------------------------------------------------------------------
    networkP  = new samson::WorkerNetwork(port, web_port);
    
    valgrindExit(12);

    // Instance of SamsonWorker object (network contains at least the number of workers)
    // -----------------------------------------------------------------------------------
    worker = new samson::SamsonWorker(networkP);

    LM_M(("Worker Running"));
    LM_D(("worker at %p", worker));
    valgrindExit(13);

    if (fg == false)
    {
        std::cout << "OK. samsonWorker is now working in background.\n";
        deamonize_close_all();

        while (true)
            sleep(10);
    }

    worker->runConsole();

    return 0;
}
