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



// Include header files for application components.
// ...

#include <set> // std::set

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

#include "samson/delilah/Delilah.h"

#include "main_DelilahQT.h"

#include <QtGui>

// QT Elements
#include "MainWindow.h"
#include "SamsonConnect.h"
#include "SamsonQueryWidget.h"

#include "DelilahConnection.h"     // DelilahConnection

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
MainWindow *mainWindow;                  // Main window....
SamsonConnect *samsonConnect;            // Connection dialog...
SamsonQueryWidget* samsonQueryWidget;    // Query widget used in the background

// Global element about this connection
DelilahConnection* delilahConnection;

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
	samson::SamsonSetup::init("","");			// Load the main setup file
	
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
	engine::ProcessManager::init(samson::SamsonSetup::shared()->getInt("general.num_processess"));
	engine::MemoryManager::init(samson::SamsonSetup::shared()->getUInt64("general.memory"));
    
    // Init the modules manager
	samson::ModulesManager::init("delilahQT");
    
}

int main(int argc, char *argv[])
{
    // Init all setup
    init( argc , argv );
    
    // Init delilah_connection object
    delilahConnection = new DelilahConnection();

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


