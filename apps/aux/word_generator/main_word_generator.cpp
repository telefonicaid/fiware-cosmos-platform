

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

#include "parseArgs/parseArgs.h"
#include "parseArgs/paConfig.h"

#include "logMsg/logMsg.h"

#include "au/string.h" // au::str()
#include "au/Cronometer.h"  // au::Cronometer
#include "au/CommandLine.h" // au::CommandLine

#include "au/CommandLine.h"     // au::CommandLine


#define PROGRESSIVE_NUMBER      20000

static const char* manShortDescription = 
"word_generator a simple toll to generate random sequences of words. Usefull in demos about word counting and topic trenddding\n";

static const char* manSynopsis =
"   [-r] [-t secs] [-l len] num_words\n"
"       [-ramdon] flag to generate randomized sequence of words\n"
"       [-repeat secs] time_to_repeat in seconds with a new wave of words\n"
"       [-l length] Number of letters of generated words ( default 9 ) \n"
"       [-alphabet alphabet size] Number of differnt letters used to generate words ( default 10 ) \n"
   "       [-progresive] flag to generate sequences of numbers in incressing order ( hit demo )\n";

int word_length;
int alphabet_length;
bool rand_flag;
int repeate_time;
int num_lines;
bool progresive;

PaArgument paArgs[] =
{
	{ "-l",           &word_length,       "",  PaInt,     PaOpt,     9, 1,  30,         "Number of letters of generated words ( default 9 )" },    
    { "-alphabet",    &alphabet_length,   "",  PaInt,     PaOpt,     10, 1, 30,         "Number of differnt letters used to generate words ( default 10 )" },
	{ "-random",      &rand_flag,         "",  PaBool,    PaOpt,  false,  false, true,  "Flag to generate completelly randomized sequence of words"},
	{ "-progressive", &progresive,        "",  PaBool,    PaOpt,  false,  false, true,  "Flag to generate sequences of numbers in incressing order"},
	{ "-repeat",      &repeate_time,      "",  PaInt,     PaOpt,     0, 0, 10000,       "time_to_repeat in seconds with a new wave of words" },    
	{ " ",            &num_lines,         "",  PaInt,     PaOpt,     0, 0, 1000000000,  "Number of words to be generated" }, 
	PA_END_OF_ARGS
};

int logFd = -1;

char word[100];

size_t progressive_counter=0;

au::Cronometer cronometer;

// Get a new random  word
void getNewWord()
{

   if ( progresive )
   {
	  size_t slot = (progressive_counter++)/PROGRESSIVE_NUMBER;
	  sprintf( word , "%lu" , slot );
	  return;
   }

    for ( int i = 0 ; i < word_length ; i++ )
        word[i] = 48 + rand()%alphabet_length;
}

int main( int argC , const char*argV[] )
{
    
    paConfig("usage and exit on any warning", (void*) true);
    
    paConfig("log to screen",                 (void*) true);
    paConfig("log to file",                   (void*) false);
    paConfig("screen line format",            (void*) "TYPE:EXEC: TEXT");
    paConfig("man shortdescription",          (void*) manShortDescription);
    paConfig("man synopsis",                  (void*) manSynopsis);
    paConfig("log to stderr",                 (void*) true);
    
    // Parse input arguments    
    paParse(paArgs, argC, (char**) argV, 1, false);
    logFd = lmFirstDiskFileDescriptor();

    
    // End of line for all the words...
    word[word_length] = '\0';

    LM_V(("Generating %d words of %d chars every %d seconds (%s)" , num_lines , alphabet_length , repeate_time , rand_flag?"Randomly":"Non randomly"));
    

    
    size_t total_num = 0;
    size_t total_size = 0;

    // Init random numbers if random specified
    if (rand_flag)
        srand( time(NULL));
    else
        srand(0);
    
    
    if( num_lines == 0)
        while( true )
        {
            getNewWord();
            printf("%s\n",word);            
        }
    
    
    while( true )
    {
        
        int local_num_lines = 0;
        
        while( local_num_lines < num_lines )
        {
            getNewWord();
            
            total_size += printf("%s\n",word);
            total_num++;
            
            local_num_lines++;
            
        }
        
        
        if( repeate_time == 0 )
            exit(1);
        else
        {
            sleep( repeate_time );
            size_t total_seconds = cronometer.diffTimeInSeconds();
            LM_V(( "Generated %s lines ( %s bytes ) in %s. Rate: %s / %s. Now sleeping %d seconds\n", 
                  au::str(total_num).c_str() , au::str(total_size).c_str(), au::time_string( total_seconds ).c_str() ,
                  au::str( (double)total_num/(double)total_seconds ,"Lines/s" ).c_str() , au::str( (double)total_size/(double)total_seconds,"Bps").c_str() , (int)repeate_time  ));
        }
        
    }
    
}
