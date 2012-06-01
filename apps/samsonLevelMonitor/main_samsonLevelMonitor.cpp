
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

#include "samson/client/SamsonClient.h"         // samson::SamsonClient

#include "common.h"

#include <QtGui>

// QT Elements
#include "MainWindow.h"

size_t buffer_size;
size_t timeOut;
double base;

char title[1024];
char composed_title[1024];
char x_title[1024];
char y_title[1024];
char concept[1024];
time_t last_timestamp;
bool graph_time;
int refresh_time;
double ntimes_real_time;
bool logX;
bool logY;


PaArgument paArgs[] =
{
        { "-title",      title,      "",            PaString, PaOpt, _i "Samson Level Monitor"   , PaNL, PaNL,       "Title of the plot"         },
        { "-x_title",    x_title,    "",            PaString, PaOpt, _i ""   , PaNL, PaNL,       "X-Title of the plot"         },
        { "-y_title",    y_title,    "",            PaString, PaOpt, _i ""   , PaNL, PaNL,       "Y-Title of the plot"         },
        { "-concept",    concept,    "CONCEPT",     PaString, PaOpt, _i "top"        , PaNL, PaNL,       "Concep to track"         },
        { "-refresh", &refresh_time, "", PaInt, PaOpt, 1, 1, 43201, "Refresh time, in secs"},
        {"-time", &graph_time, "", PaBool, PaOpt, false, false, true, "Presents graph as a time series"},
        {"-nr", &ntimes_real_time, "", PaDouble, PaOpt, 1.0, 0.1, 1000, "ntimes real time of data source"},
        { "-base",       &base,      "",            PaDouble, PaOpt,      1.0        , 1.0, 1000000.0,       "Base to divide all numbers ( example 1000 1000000 .... )"   },
        {"-logY", &logY, "", PaBool, PaOpt, false, false, true, "Logarithmic Y axis"},
        {"-logX", &logX, "", PaBool, PaOpt, false, false, true, "Logarithmic X axis"},
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
au::map<std::string , au::ContinuousValueCollection<time_t> > time_collections;
std::vector<std::string> concepts; // Vector with all the concepts

void process_command( std::string line )
{


    // Remove returns at the end
    //    while ( line[ line.size()-1 ] == '\n' )
    //    line.erase( line.size()-1 );

    au::CommandLine cmd;
    cmd.parse( line );

    time_t timestamp = 0;

    if( cmd.get_num_arguments() < 2 )
    {
        LM_W(("Input line without two parameters, invalid format"));
        return; // Not valid format
    }

    std::string name = cmd.get_argument(0);
    double value =  atof( cmd.get_argument(1).c_str() ) / base;

    if (cmd.get_num_arguments() > 2)
    {
        timestamp = atoi(cmd.get_argument(2).c_str());
    }

    //LM_M(("Converting %s --> %f at timestamp:%lu", cmd.get_argument(1).c_str()  , value, static_cast<unsigned long>(timestamp) ));

    //std::cout << au::str( "Processing '%s' %s=%f\n" , line.c_str() , name.c_str() , value );

    for ( size_t i = 0 ; i < concepts.size() ; i ++ )
    {
        if ( name == concepts[i] )
        {
            LM_M(("Adding name:'%s' with value:%lf and timestamp:%s", name.c_str(), value, ctime(&timestamp)));
            au::TokenTaker tt(&token_value_collections); // We are the only element right now, but in the future we can have multiple
            au::ContinuousValueCollection<double>* vc = value_collections.findOrCreate( name );
            vc->set( value );
            au::ContinuousValueCollection<time_t>* tc = time_collections.findOrCreate( name );
            tc->set( timestamp );

            if (graph_time)
            {
            if (timestamp > last_timestamp)
            {
                struct tm tm_calendar;

                if (gmtime_r(&timestamp, &tm_calendar) != NULL)
                {
                    char timestamp_str[81];
                    asctime_r(&tm_calendar, timestamp_str);
                    timestamp_str[strlen(timestamp_str)-1] = '\0';
                    sprintf(composed_title, "%s at %s (%.0lf%% real_time, refresh:%d secs)", title, timestamp_str, ntimes_real_time*100.0, refresh_time);
                    last_timestamp = timestamp;
                }
                else
                {
                    sprintf(composed_title, "%s (%.0lf%% real_time)", title, ntimes_real_time*100.0);
                }
            }
            }
            else
            {
                sprintf(composed_title, "%s (refresh:%d secs)", title, refresh_time);
            }
        }
        else if (value_collections.findInMap(concepts[i] ) == NULL)
        {

            au::ContinuousValueCollection<double>* vc = value_collections.findOrCreate(concepts[i] );
            vc->set(0.0);
        }
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
        {
            LM_X(0,("No more commands to process at stdin"));
        }

        process_command( line );

    }
    return NULL;
}


int main( int argC ,  char *argV[] )
{
    paConfig("usage and exit on any warning", (void*) true);

    paConfig("log to screen",                 (void*) true);
    paConfig("log to file",                   (void*) true);
    paConfig("screen line format",            (void*) "TYPE:EXEC: TEXT");
    paConfig("man shortdescription",          (void*) manShortDescription);

    // Parse input arguments
    paParse(paArgs, argC, (char**) argV, 1, false);
    logFd = lmFirstDiskFileDescriptor();

    if ( strcmp( concept,"main") == 0)
        LM_W(("No concept specified with -concept option. Tracking 'main' concept...."));

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
