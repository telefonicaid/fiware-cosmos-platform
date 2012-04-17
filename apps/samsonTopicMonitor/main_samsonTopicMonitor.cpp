
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

#include "au/containers/map.h"
#include "au/RRT.h"                             // au::ValueCollection 
#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"
#include "au/string.h"                          // au::str()
#include "au/ThreadManager.h"

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
	{ "-concept",     concept,    "CONCEPT",     PaString, PaOpt, _i "top"        , PaNL, PaNL,       "Concept to track"         },
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
// Custom operations to parse input lines
// --------------------------------------------------------------------------------

au::Token token_web_content("value_collections");
std::string web_content;
std::vector<Topic> topics;


void process_command( std::string line )
{
    au::TokenTaker tt(&token_web_content);

    // Split input line in words
    std::vector<std::string> words;
    au::split( line , ' ' , words );

    // Not the right concept
	if( ( words.size() == 0 ) || ( words[0] != concept) )
	{
	   LM_M(("Skipping line with concept %s ( selecting concept %s)" , words[0].c_str() , concept ));
	   return;
	}

    // Clear list of topics
    topics.clear();

	int num_concepts = (words.size()-1) / 2;  // Format word number

	// Maximum number of hits
	size_t max_num = 0;

	std::ostringstream output_web;
	for (int i = 0 ; i < num_concepts ; i++ )
	{
	   std::string concept = words[1+i*2];
	   size_t num = atoll( words[1+i*2 +1].c_str() );
	   
	   topics.push_back( Topic( concept, num) );
	   if( max_num < num )
		  max_num = num;
	}
    

	for ( int i = 0 ; i < num_concepts ; i++ )
	{
	   int font_size = 20.0 * ((double) topics[i].num) / ((double) max_num);
	   if ( font_size < 2 )
		  font_size = 2;

	   //output_web << "<p style=\"font-size:" << font_size << "px;\">" << topics[i].concept << "</p> ";
        output_web << "<font size=" << font_size << ">" << topics[i].concept  << " ( " << au::str(topics[i].num) << " )  </font> ";
	}

    // Parse content
	   web_content = output_web.str();;
    
    return;

    
}

void* process_income_blocks(void*)
{
    // Free resources automatically when this thread finish
    pthread_detach(pthread_self());    
    
    while( true )
    {
        LM_V(("Start reading lines from stdin..."));
        
        char line[100000];
        if( fgets( line, 100000 , stdin ) == NULL )
            LM_X(0,("No more commands to process at stdin"));

        au::remove_return_chars(line);

        LM_V(("Process income line: %s" , line));
        
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
        LM_W(("No concept specified with -concept optin. Tracking 'main' concept...."));
    
    
    // Run the thread to update incoming blocks
    pthread_t t;
    au::ThreadManager::shared()->addThread( "main::SamsonTopicMonitor" ,&t, NULL, process_income_blocks, NULL);
    
	// Create the app ( QT library )
    app =  new QApplication(argC, argV);

    // Main window ( hide at start )
    mainWindow = new MainWindow();
    mainWindow->show();

    // Main QT loop
    return app->exec();
	
}
