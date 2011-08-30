/* ****************************************************************************
*
* FILE            main_Delilah.cpp
*
* AUTHOR          Andreu Urruela
*
* DATE            July 2011
*
* DESCRIPTION
*
* Main file for the "delilah" console app
*
*/
#include "parseArgs/parseArgs.h"

#include "au/Format.h"
#include "au/LockDebugger.h"            // au::LockDebugger


#include "engine/MemoryManager.h"
#include "engine/Engine.h"
#include "engine/DiskManager.h"
#include "engine/ProcessManager.h"

#include "samson/module/ModulesManager.h"       // samson::ModulesManager

#include "samson/common/samsonVersion.h"
#include "samson/common/SamsonSetup.h"

#include "samson/network/Network2.h"
#include "samson/network/EndpointManager.h"
#include "samson/network/Packet.h"


#include "samson/delilah/DelilahConsole.h"
#include "samson/delilah/DelilahMonitorization.h"


/* ****************************************************************************
*
* Option variables
*/
char             controller[80];
int				 memory_gb;
int				 load_buffer_size_mb;
char			 commandFileName[1024];
bool             monitorization;


#define LOC "localhost"
/* ****************************************************************************
*
* parse arguments
*/
PaArgument paArgs[] =
{
	{ "-controller",       controller,           "CONTROLLER",       PaString, PaOpt, _i LOC, PaNL, PaNL, "controller IP:port"         },
	{ "-memory",           &memory_gb,           "MEMORY",           PaInt,    PaOpt,      1,    1,  100, "memory in GBytes"           },
	{ "-load_buffer_size", &load_buffer_size_mb, "LOAD_BUFFER_SIZE", PaInt,    PaOpt,     64,   64, 2048, "load buffer size in Mbytes" },
	{ "-f",                 commandFileName,     "FILE_NAME",        PaString, PaOpt,  _i "", PaNL, PaNL, "File with commands to run"  },
	{ "-monitorization",    &monitorization,        "MONITORIZAITON",      PaBool,    PaOpt,  false, false,  true,  "Run monitorization tool"   },

	PA_END_OF_ARGS
};



class Delilah : public samson::PacketReceiverInterface, public samson::PacketSenderInterface
{
private:
	samson::NetworkInterface* networkP;

public:
	Delilah(samson::NetworkInterface* network);
	void receive(samson::Packet* packet);
	void notificationSent(size_t id, bool success);
};



Delilah::Delilah(samson::NetworkInterface* network)
{
	networkP = network;
}



void Delilah::receive(samson::Packet* packet)
{
	LM_M(("Got a packet from endpoint %d (msg code '%s')", packet->fromId, samson::Message::messageCode(packet->msgCode)));
}



void Delilah::notificationSent(size_t id, bool success)
{
	LM_M(("Got a notification that a packet has been sent to endpoint %d", id));
}



/* ****************************************************************************
*
* logFd - file descriptor for log file used in all libraries
*/
int logFd = -1;



/* ****************************************************************************
*
* man texts -
*/
static const char* manSynopsis         = " [OPTION]";
static const char* manShortDescription = "delilah is the user scripting environment of the samson platform, ...\n\n"
   "Complete list of options:\n";
static const char* manDescription      =
   "delilah is used to interact with the samson platform.\n"
   "An impressive number of commands are available in delilah ...\n";

static const char* manExitStatus    = "0      if OK\n 1-255  error\n";
static const char* manAuthor        = "Written by Andreu Urruela, Ken Zangelin and J.Gregorio Escalada.";
static const char* manReportingBugs = "bugs to samson-bug-report@tid.es\nSamson home page: <http://www.tid.es/products/samson>";
static const char* manCopyright     = "Copyright (C) 2011 Telefonica Investigacion y Desarrollo";
static const char* manVersion       = SAMSON_VERSION;



/* ****************************************************************************
*
* main - 
*/
int main(int argC, const char *argV[])
{
    
	paConfig("prefix",                        (void*) "SSW_");
	paConfig("usage and exit on any warning", (void*) true);
	paConfig("log to screen",                 (void*) false);
	paConfig("log file line format",          (void*) "TYPE:DATE:EXEC-AUX/FILE[LINE] FUNC: TEXT");
	paConfig("log to file",                   (void*) true);

    paConfig("man synopsis",                  (void*) manSynopsis);
    paConfig("man shortdescription",          (void*) manShortDescription);
    paConfig("man description",               (void*) manDescription);
    paConfig("man exitstatus",                (void*) manExitStatus);
    paConfig("man author",                    (void*) manAuthor);
    paConfig("man reportingbugs",             (void*) manReportingBugs);
    paConfig("man copyright",                 (void*) manCopyright);
    paConfig("man version",                   (void*) manVersion);

	paParse(paArgs, argC, (char**) argV, 1, false);
	lmAux((char*) "father");
	logFd = lmFirstDiskFileDescriptor();
	
    // Make sure this singlelton is created just once
    au::LockDebugger::shared();

	samson::SamsonSetup::init();			// Load the main setup file
	
	// Setup parameters from command line ( this is delilah so memory and load buffer size are configurable from command line )
    size_t _memory = (size_t) memory_gb * (size_t) (1024*1024*1024);
    std::stringstream memory;
    memory << _memory;

    size_t _load_buffer_size = (size_t) load_buffer_size_mb * (size_t) (1024*1024);
    std::stringstream load_buffer_size;
    load_buffer_size << _load_buffer_size;
    
	samson::SamsonSetup::shared()->setValueForParameter("general.memory", memory.str() );
    samson::SamsonSetup::shared()->setValueForParameter("load.buffer_size",  load_buffer_size.str() );


	engine::Engine::init();
	engine::DiskManager::init(1);
	engine::ProcessManager::init(samson::SamsonSetup::getInt("general.num_processess"));
	engine::MemoryManager::init(samson::SamsonSetup::getUInt64("general.memory"));
    
	samson::ModulesManager::init();         // Init the modules manager
	
	// Initialize the network element for delilah
	samson::Network2*        networkP  = new samson::Network2( samson::Endpoint2::Delilah, controller );

	networkP->runInBackground();

	std::cout << "\nConnecting to SAMSOM controller " << controller << " ...";
    std::cout.flush();
    
	//
	// What until the network is ready
	//
	while (!networkP->ready())
		usleep(1000);
	std::cout << " OK\n";

	std::cout << "Connecting to all workers ...";
    std::cout.flush();
    
	//
	// Ask the Controller for the platform process list
	//
	// First, give controller some time for the interchange of Hello messages
	//
	samson::Packet*  packetP  = new samson::Packet(samson::Message::Msg, samson::Message::ProcessVector);

	LM_TODO(("I should probably go through NetworkInterface here ..."));
	networkP->epMgr->controller->send( packetP );

	//
	// What until the network is ready II
	//
	while (!networkP->ready(true))
		sleep(1);
	std::cout << " OK\n";


	// Create a DelilahControler once network is ready
	samson::DelilahConsole* delilahConsole = NULL;
    samson::DelilahMonitorization* delilahMonitorization = NULL;
	
    if( !monitorization )
        delilahConsole = new samson::DelilahConsole(networkP);
    else
        delilahMonitorization = new samson::DelilahMonitorization(networkP);
	
    // Not necessary anymore. Engine automatically starts with "init" call
	//engine::Engine::runInBackground();
	
    // Special mode for file-based commands
    // ----------------------------------------------------------------
    
	if ( strcmp( commandFileName,"") != 0 )
	{
        if( !delilahConsole )
            LM_X(1, ("It is not valid to run monitorization with commands"));
        
		FILE *f = fopen( commandFileName , "r" );
		if( !f )
		{
			LM_E(("Error opening commands file %s", commandFileName));
			exit(0);
		}
		
		char line[1024];
        
		//LM_M(("Processing commands file %s", commandFileName ));
		while( fgets(line, sizeof(line), f) )
		{
			// Remove the last return of a string
			while( ( strlen( line ) > 0 ) && ( line[ strlen(line)-1] == '\n') > 0 )
				line[ strlen(line)-1]= '\0';
			
			//LM_M(("Processing line: %s", line ));
			size_t id = delilahConsole->runAsyncCommand( line );
			
			if( id != 0)
			{
				//LM_M(("Waiting until delilah-component %ul finish", id ));
				// Wait until this operation is finished
				while (delilahConsole->isActive( id ) )
					sleep(1);
			}
		}
		
		fclose(f);
		
        
		LM_M(("samsonLocal exit correctly"));
		
		exit(0);
	}    
    
    if( delilahConsole )
        delilahConsole->run();
    
    if( delilahMonitorization )
        delilahMonitorization->run();
    
    
}
