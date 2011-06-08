#include "parseArgs/parseArgs.h"
#include "samson/delilah/DelilahConsole.h"
#include "samson/common/SamsonSetup.h"
#include "au/Format.h"
#include "engine/MemoryManager.h"
#include "engine/Engine.h"
#include "samson/network/Network2.h"
#include "samson/network/EndpointManager.h"
#include "samson/network/Packet.h"



/* ****************************************************************************
*
* Option variables
*/
char             controller[80];
int				 memory_gb;
int				 load_buffer_size_mb;
char			 commandFileName[1024];



#define LOC "localhost:1234"
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
* main - 
*/
int main(int argC, const char *argV[])
{
	paConfig("prefix",                        (void*) "SSW_");
	paConfig("usage and exit on any warning", (void*) true);
	paConfig("log to screen",                 (void*) false);
	paConfig("log file line format",          (void*) "TYPE:DATE:EXEC-AUX/FILE[LINE] FUNC: TEXT");
	paConfig("log to file",                   (void*) true);

	paParse(paArgs, argC, (char**) argV, 1, false);
	lmAux((char*) "father");
	logFd = lmFirstDiskFileDescriptor();
	
	samson::SamsonSetup::load();			// Load the main setup file
	
	// Setup parameters from command line ( this is delilah so memory and load buffer size are configurable from command line )
	samson::SamsonSetup::shared()->memory			= (size_t) memory_gb * (size_t) (1024*1024*1024);
	samson::SamsonSetup::shared()->load_buffer_size = (size_t) load_buffer_size_mb * (size_t) (1024*1024);

	engine::Engine::init();
	// Goyo. Groping in the dark (blind sticks for an easier translation)
	engine::MemoryManager::init(  samson::SamsonSetup::shared()->memory );
	// Goyo. End of groping in the dark

	
	std::cout << "Waiting for network connection ...";
	
	// Initialize the network element for delilah
	samson::EndpointManager* epMgr     = new samson::EndpointManager(samson::Endpoint2::Delilah, controller);
	samson::Network2*        networkP  = new samson::Network2(epMgr);

	networkP->runInBackground();



	//
	// What until the network is ready
	//
	std::cout << "Awaiting network ready";
	while (!networkP->ready())
		usleep(100000);
	std::cout << "OK\n";

	//
	// Ask the Controller for the platform process list
	//
	// First, give controller some time for the interchange of Hello messages
	//
	samson::Packet*  packetP  = new samson::Packet(samson::Message::Msg, samson::Message::ProcessVector);

	LM_TODO(("I should probably go through NetworkInterface here ..."));
	epMgr->controller->send(packetP);

	// Create a DelilahControler once network is ready
	samson::DelilahConsole delilahConsole(networkP);
	
	
	engine::Engine::runInBackground();
	
    
	if ( strcmp( commandFileName,"") != 0 )
	{
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
			size_t id = delilahConsole.runAsyncCommand( line );
			
			if( id != 0)
			{
				//LM_M(("Waiting until delilah-component %ul finish", id ));
				// Wait until this operation is finished
				while (delilahConsole.isActive( id ) )
					sleep(1);
			}
		}
		
		fclose(f);
		
        
		LM_M(("samsonLocal exit correctly"));
		
		exit(0);
	}    
    
	delilahConsole.run();
}
