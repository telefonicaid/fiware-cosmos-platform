
/*
 
 samsonPopQT
 
 Example app using samsonClient lib
 It listen std input and push content to the samson cluster indicated by parameters
 
 AUTHOR: Andreu Urruela
 
 */


#include <stdio.h>      // printf
#include <stdlib.h>     // exit
#include <string.h>     // memcpy
#include <iostream>     // std::cout

#include "parseArgs/parseArgs.h"
#include "logMsg/logMsg.h"

#include "au/map.h"
#include "au/RRT.h"                             // au::ValueCollection 
#include "au/Token.h"
#include "au/TokenTaker.h"
#include "au/string.h"                          // au::str()

#include "samson/client/SamsonClient.h"         // samson::SamsonClient

#include "common.h"

#include <QtGui>

// QT Elements
#include "MainWindow.h"

size_t buffer_size;
size_t timeOut;
double base;

char title[1024];
char x_title[1024];
char y_title[1024];
char concept[1024];

PaArgument paArgs[] =
{
	{ "-title",      title,      "",            PaString, PaOpt, _i "Samson Level Monitor"   , PaNL, PaNL,       "Title of the plot"         },
	{ "-x_title",    x_title,    "",            PaString, PaOpt, _i ""   , PaNL, PaNL,       "X-Title of the plot"         },
	{ "-y_title",    y_title,    "",            PaString, PaOpt, _i ""   , PaNL, PaNL,       "Y-Title of the plot"         },
	{ "-concept",    concept,    "CONCEPT",     PaString, PaOpt, _i "top"        , PaNL, PaNL,       "Concep to track"         },
	{ "-base",       &base,      "",            PaDouble, PaOpt,      1.0        , 1.0, 1000000.0,       "Base to divide all numbers ( example 1000 1000000 .... )"   },
    PA_END_OF_ARGS
};

static const char* manShortDescription = 
"samsonTopicMonitor is an easy-to-use software to display topics on screen.\n";

int logFd = -1;

QApplication *app;                    // Global QT application object

// Top global QT Elements
MainWindow *mainWindow;               // Main window....

// --------------------------------------------------------------------------------
// Custom line process routine
// --------------------------------------------------------------------------------

// Map of values collections ( updated from 
au::Token token_value_collections("value_collections");
au::map<std::string , au::ContinuousValueCollection<double> > value_collections;
std::vector<std::string> concepts; // Vector with all the concepts

void process_command( std::string line )
{
    
    
    // Remove returns at the end
    //    while ( line[ line.size()-1 ] == '\n' )
    //    line.erase( line.size()-1 );
    
    au::CommandLine cmd;
    cmd.parse( line );
    
    if( cmd.get_num_arguments() < 2 )
        return; // Not valid format
    
    std::string name = cmd.get_argument(0);
    double value =  atof( cmd.get_argument(1).c_str() ) / base;
    
    LM_V(("Converting %s --> %f", cmd.get_argument(1).c_str()  , value ));
    
    //std::cout << au::str( "Processing '%s' %s=%f\n" , line.c_str() , name.c_str() , value );
    
    for ( size_t i = 0 ; i < concepts.size() ; i ++ )
        if ( name == concepts[i] )
        {
            au::TokenTaker tt(&token_value_collections); // We are the only element rigth now, but in the future we can have multiple
            au::ContinuousValueCollection<double>* vc = value_collections.findOrCreate( name );
            vc->set( value );
        }
}



void* process_income_blocks(void*)
{
    // Free resources automatically when this thread finish
    pthread_detach(pthread_self());
    
    while( true )
    {
        char line[100000];
        if( fgets( line, 100000 , stdin ) == NULL )
            LM_X(0,("No more commands to process at stdin"));

        process_command( line );
        
    }
    return NULL;
}


int main( int argC ,  char *argV[] )
{
    paConfig("usage and exit on any warning", (void*) true);
    
    paConfig("log to screen",                 (void*) true);
    paConfig("log to file",                   (void*) false);
    paConfig("screen line format",            (void*) "TYPE:EXEC: TEXT");
    paConfig("man shortdescription",          (void*) manShortDescription);
    
    // Parse input arguments    
    paParse(paArgs, argC, (char**) argV, 1, false);
    logFd = lmFirstDiskFileDescriptor();
    
    if ( strcmp( concept,"main") == 0)
        LM_W(("No concept specified with -concept optin. Traking 'main' concept...."));

    // Split the concept in concepts to track all of them
    au::split( concept , ',' , concepts );   

    
    LM_V(("------------------------------------------------"));
    LM_V(("SETUP"));
    LM_V(("------------------------------------------------"));
    LM_V(("Base %f",base));
    LM_V(("concept %s (%d concepts)",concept,(int)concepts.size()));
    LM_V(("------------------------------------------------"));
    
    
    // Run the thread to update incomming blocks
    pthread_t t;
    pthread_create(&t, NULL, process_income_blocks, NULL);

	// Create the app ( QT library )
    app =  new QApplication(argC, argV);

    // Main window ( hide at start )
    mainWindow = new MainWindow();
    mainWindow->show();

    // Main QT loop
    return app->exec();
	
}
