
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
#include "au/RRT.h"             // au::ValueCollection 
#include "au/Token.h"
#include "au/string.h"              // au::str()

#include "samson/client/SamsonClient.h"         // samson::SamsonClient

#include <QtGui>

// QT Elements
#include "MainWindow.h"

size_t buffer_size;
size_t timeOut;

char controller[1024];
char queue_name[1024];
char title[1024];
char concept[1024];

PaArgument paArgs[] =
{
	{ "-controller",  controller, "CONTROLLER",  PaString, PaOpt, _i "localhost"  , PaNL, PaNL,       "controller IP:port"         },
	{ "-title",       title,      "TITLE",       PaString, PaOpt, _i "no title"   , PaNL, PaNL,       "Title of the plot"         },
	{ "-concept",     concept,    "CONCEPT",     PaString, PaOpt, _i "main"       , PaNL, PaNL,       "Concep to track"         },
	{ " ",            queue_name, "QUEUE",       PaString, PaOpt,  (long) "null"  , PaNL,   PaNL,     "name of the queue to push data"         },
    PA_END_OF_ARGS
};

static const char* manShortDescription = 
"samsonPop is an easy-to-use client to receive data from a particular queue in a SAMSON system.\n";

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


// Map of values collections ( updated from 
au::map<std::string , au::ContinuousValueCollection<double> > value_collections;
au::Token token_value_collections("value_collections");


// Instance of the client to connect to SAMSON system
samson::SamsonClient samson_client;


size_t getLineSize( char * data , size_t max)
{
    for ( size_t i=0;i < max ; i++)
        if( data[i] == '\n' )
            return i+1;
    return max;
}

void process_command( std::string &line )
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
        
        samson::SamsonClientBlockInterface *block = samson_client.getNextBlock( queue_name );
        
        if( block )
        {
            // Process block
            size_t size = block->getTXTBufferSize();
            char *data  = block->getTXTBufferPointer();
            
            //std::cout << au::str("Processing block with %lu bytes\n" , size);
            
            size_t p = 0;
            
            while( p < size )
            {
                
                size_t line_size = getLineSize( data+p , size-p );
                
                //std::cout << au::str("Processing line with %lu bytes\n" , line_size);
                
                std::string line;
                line.append( data+p , line_size );
                
                process_command( line );
                
                p+= line_size;
            }
            
            
            delete block;
        }
        else
        {
            usleep(100000);
        }
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
    
    if(strcmp(queue_name,"null") == 0 )
       LM_X(1,("Please specify a queue to receive data from"));
    
    if ( strcmp( concept,"main") == 0)
        LM_W(("No concept specified with -concept optin. Traking 'main' concept...."));
    
    // Set 1G RAM for uploading content
    samson_client.setMemory( 1024*1024*1024 );
    
    LM_M(("Connecting to %s ..." , controller));
    
    // Init connection
    if( !samson_client.init( controller ) )
    {
        fprintf(stderr, "Error connecting with samson cluster: %s\n" , samson_client.getErrorMessage().c_str() );
        exit(0);
    }
    LM_M(("Conection to %s OK" , controller));

    
    // Connect to a particular queue
    LM_M(("Connecting to queue %s" , queue_name ));
    samson_client.connect_to_queue( queue_name );

    
    
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


/*
    while( true )
    {
        
        samson::SamsonClientBlockInterface *block = samson_client.getNextBlock( queue_name );
        
        if( block )
        {
		   block->print_header();
		   delete block;
        }
        else
            usleep(100000);
        
    }
*/  
	
}
