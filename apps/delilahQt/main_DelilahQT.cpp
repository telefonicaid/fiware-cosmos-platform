


// Include header files for application components.
// ...


#include "parseArgs/parseArgs.h"

#include "au/string.h"
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

#include "samson/delilah/Delilah.h"     // samson::Delilah

#include <QtGui>

// QT Elements
#include "MainWindow.h"
#include "SamsonConnect.h"
#include "SamsonQueryWidget.h"

int logFd;

/* ****************************************************************************
 *
 * Option variables
 */
int				 memory_gb;
int				 load_buffer_size_mb;

#define LOC "localhost"
/* ****************************************************************************
 *
 * parse arguments
 */

PaArgument paArgs[] =
{
	{ "-memory",           &memory_gb,           "MEMORY",           PaInt,    PaOpt,      1,    1,  100, "memory in GBytes"           },
	{ "-load_buffer_size", &load_buffer_size_mb, "LOAD_BUFFER_SIZE", PaInt,    PaOpt,     64,   64, 2048, "load buffer size in Mbytes" },
	PA_END_OF_ARGS
};



QApplication *app;                    // Global QT application object

// Top global QT Elements
MainWindow *mainWindow;               // Main window....
SamsonConnect *samsonConnect;         // Connection dialog...
SamsonQueryWidget* samsonQueryWidget; // Query widget used in the background

// Top Network & Delilah object used
samson::Network2* networkP;
samson::Delilah *delilah;


void init( int argC, char *argV[] )
{
    // Parser Argument setup
    
	paConfig("prefix",                        (void*) "SSW_");
	paConfig("usage and exit on any warning", (void*) true);
	paConfig("log to screen",                 (void*) false);
	paConfig("log file line format",          (void*) "TYPE:DATE:EXEC-AUX/FILE[LINE](p.PID)(t.TID) FUNC: TEXT");
	paConfig("log to file",                   (void*) true);
/*    
    paConfig("man synopsis",                  (void*) manSynopsis);
    paConfig("man shortdescription",          (void*) manShortDescription);
    paConfig("man description",               (void*) manDescription);
    paConfig("man exitstatus",                (void*) manExitStatus);
    paConfig("man author",                    (void*) manAuthor);
    paConfig("man reportingbugs",             (void*) manReportingBugs);
    paConfig("man copyright",                 (void*) manCopyright);
    paConfig("man version",                   (void*) manVersion);
  */  
	paParse(paArgs, argC, (char**) argV, 1, false);
	lmAux((char*) "father");
	logFd = lmFirstDiskFileDescriptor();
	
    // Make sure this singlelton is created just once
    au::LockDebugger::shared();
	samson::SamsonSetup::init();			// Load the main setup file
	
	// Memory manager setup
    size_t _memory = (size_t) memory_gb * (size_t) (1024*1024*1024);
    std::stringstream memory;
    memory << _memory;
	samson::SamsonSetup::shared()->setValueForParameter("general.memory", memory.str() );
    
    // Load buffer setup
    size_t _load_buffer_size = (size_t) load_buffer_size_mb * (size_t) (1024*1024);
    std::stringstream load_buffer_size;
    load_buffer_size << _load_buffer_size;
    samson::SamsonSetup::shared()->setValueForParameter("load.buffer_size",  load_buffer_size.str() );
    
    // Init all engine-related systems
	engine::Engine::init();
	engine::DiskManager::init(1);
	engine::ProcessManager::init(samson::SamsonSetup::getInt("general.num_processess"));
	engine::MemoryManager::init(samson::SamsonSetup::getUInt64("general.memory"));
    
    // Init the modules manager
	samson::ModulesManager::init();         
    
}


bool connect( std::string controller )
{
    if( networkP )
        return false; // Still connecting with a previous controller....
    
	// Initialize the network element for delilah
	networkP  = new samson::Network2( samson::Endpoint2::Delilah, controller.c_str() );
	networkP->runInBackground();
    
	std::cerr << "\nConnecting to SAMSOM controller " << controller << " ...";
    std::cerr.flush();
    
	//
	// What until the network is ready
	//
	while (!networkP->ready())
		usleep(1000);
	std::cout << " OK\n";
	LM_M(("\nConnecting to SAMSOM controller %s ... OK", controller.c_str()));
    
	std::cerr << "Connecting to all workers ...";
    std::cerr.flush();
    
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
	LM_M(("\nConnecting to all workers ... OK"));
    
    // Creating delilah object
    delilah = new samson::Delilah( networkP , true );
    
    
    // Hide the connection pannel
    samsonConnect->hide();
    
    //Show main window
    mainWindow->show();
    
    
    return true;
}



int main(int argc, char *argv[])
{
    init( argc , argv );
    
    // Init delilah object
    delilah = NULL;
    
    // Create the app
    app =  new QApplication(argc, argv);
    
    // Main window ( hide at start )
    mainWindow = new MainWindow();
    mainWindow->hide();
    
    // Create the connection dialog
    samsonConnect = new SamsonConnect();
    samsonConnect->show();
    
    // Query widget initially hidden
    samsonQueryWidget = new SamsonQueryWidget();
    samsonQueryWidget->hide();
    
    // Main QT loop
    return app->exec();
    
}


