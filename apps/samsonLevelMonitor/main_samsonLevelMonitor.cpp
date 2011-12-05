
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

char title[1024];
char concept[1024];

PaArgument paArgs[] =
{
	{ "-title",       title,      "TITLE",       PaString, PaOpt, _i "no title"   , PaNL, PaNL,       "Title of the plot"         },
	{ "-concept",     concept,    "CONCEPT",     PaString, PaOpt, _i "top"        , PaNL, PaNL,       "Concep to track"         },
    PA_END_OF_ARGS
};

static const char* manShortDescription = 
"samsonTopicMonitor is an easy-to-use software to display topics on screen.\n";

int logFd = -1;

void find_and_replace( std::string &source, const std::string find, std::string replace ) {
   size_t j;
   for ( ; (j = source.find( find )) != std::string::npos ; ) {
	  source.replace( j, find.length(), replace );
   }
}

void literal_string( std::string& txt )
{
   std::string slash = "\n"; 
   std::string replace_slash = "\\n";
   find_and_replace( txt , slash , replace_slash );   
}

size_t full_read( int fd , char* data , size_t size)
{
    size_t read_size = 0;
    
    while( read_size < size )
    {
        ssize_t t = read( fd , data+read_size , size - read_size );
        
        if( t==-1)
            LM_X(1,("Error reading input data"));
        
        if( t == 0)
            break;
        else
            read_size+=t;
    }
    
    return read_size;    
}

QApplication *app;                    // Global QT application object

// Top global QT Elements
MainWindow *mainWindow;               // Main window....


// Content to display on web display 
// --------------------------------------------------------------------------------

size_t getLineSize( char * data , size_t max)
{
    for ( size_t i=0;i < max ; i++)
        if( data[i] == '\n' )
            return i+1;
    return max;
}

// --------------------------------------------------------------------------------
// Custom line process routine
// --------------------------------------------------------------------------------

// Map of values collections ( updated from 
au::Token token_value_collections("value_collections");
au::map<std::string , au::ContinuousValueCollection<double> > value_collections;


void process_command( std::string line )
{
    
    // Remove returns at the end
    while ( line[ line.size()-1 ] == '\n' )
        line.erase( line.size()-2 );
    
    au::CommandLine cmd;
    cmd.parse( line );
    
    if( cmd.get_num_arguments() < 2 )
        return; // Not valid format
    
    std::string name = cmd.get_argument(0);
    double value =  atof( cmd.get_argument(1).c_str() );
    
    //std::cout << au::str( "Processing '%s' %s=%f\n" , line.c_str() , name.c_str() , value );
    if( name == concept) // Only accept selected concept
    {
        au::TokenTaker tt(&token_value_collections); // We are the only element rigth now, but in the future we can have multiple
        au::ContinuousValueCollection<double>* vc = value_collections.findOrCreate( name );
        vc->set( value );
    }    
}



void* process_income_blocks(void*)
{
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
