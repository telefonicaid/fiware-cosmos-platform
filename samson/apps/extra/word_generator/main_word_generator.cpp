

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
int max_num_lines;
bool progresive;
int max_rate; // Max rate of words per second
PaArgument paArgs[] =
{
	{ "-l",           &word_length,       "",  PaInt,     PaOpt,     9, 1,  30,             "Number of letters of generated words ( default 9 )" },    
    { "-alphabet",    &alphabet_length,   "",  PaInt,     PaOpt,     10, 1, 30,             "Number of differnt letters used to generate words ( default 10 )" },
	{ "-random",      &rand_flag,         "",  PaBool,    PaOpt,  false,  false, true,      "Flag to generate completelly randomized sequence of words"},
	{ "-progressive", &progresive,        "",  PaBool,    PaOpt,  false,  false, true,      "Flag to generate sequences of numbers in incressing order"},
	{ "-rate",        &max_rate,          "",  PaInt,     PaOpt,     0, 0, 10000000000,     "Max rate in words / second" },    
	{ " ",            &max_num_lines,         "",  PaInt,     PaOpt,     0, 0, 1000000000,  "Number of words to be generated" }, 
	PA_END_OF_ARGS
};

int logFd = -1;

char word[100];                  // Place for this word
int progressive_word_slots[100]; // Indexes to generate progressive words

au::Cronometer cronometer;

// Get a new random  word
void getNewWord()
{

   if ( progresive )
   {
       for ( int i = 0 ; i < word_length ; i++ )
           word[i] = 48 + progressive_word_slots[i];
       
       // Increase counter...
       progressive_word_slots[word_length-1]++;
       
       int pos = word_length-1;
       while( (pos>=0) && ( progressive_word_slots[pos] >= alphabet_length ) )
       {
           progressive_word_slots[pos]=0;
           if( pos > 0 )
               progressive_word_slots[pos-1]++;
           pos--;
       }
       
       return;
   }

    for ( int i = 0 ; i < word_length ; i++ )
        word[i] = 48 + rand()%alphabet_length;
}


class BufferToScreen
{
    char *buffer;
    size_t max_size;
    size_t size;
    
public:
    
    BufferToScreen( size_t _max_size )
    {
        max_size = _max_size;
        buffer = (char*) malloc( max_size );
        size = 0;
    }
    
    void append( char* data , int len )
    {
        if( (size + len ) > max_size )
            flush();
        
        memcpy(buffer+size, data, len);
        size += len;
    }
    
    void flush()
    {
        size_t w = write(1, buffer, size);
		if( w != size )
		   LM_X(1,("Problem writing %lu bytes to the screen", size));
        size = 0;
    }
    
};


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

    // Init progressive
    if ( progresive )
        for ( int i = 0 ; i < word_length ; i++ )
            progressive_word_slots[i]=0;

    // End of line for all the words...
    word[word_length] = '\n';
    word[word_length+1] = '\0';
    
    size_t num_lines = 0;
    size_t total_size = 0;

    // Init random numbers if random specified
    if (rand_flag)
        srand( time(NULL));
    else
        srand(0);
    
    // Time to show a message on screen in verbose mode
    size_t last_message_time=0;

    // Buffer to flsh data to screen in batches
    BufferToScreen buffer_to_screen( 10000 );
    
    double lines_per_second = 0;
    
    // Generate continuously...
    while( true )
    {
        // Check the limit of generated words
        if( max_num_lines > 0 )
		   if( num_lines >= (size_t) max_num_lines )
                break; 
        
        // Get new word
        getNewWord();

        // Length of this word
        int word_len = strlen(word);

        // Append to the screen
        buffer_to_screen.append( word , word_len );
        
        // Counter of bytes and words
        total_size += word_len;
        num_lines++;

        // This avoid exesive calls to timeout
        if( lines_per_second > 100000 )
        {
            size_t num_continue = lines_per_second/100;
            if( (num_lines%num_continue) != 0 )
                continue;
        }

        // Flush accumulated buffer so far
        buffer_to_screen.flush();
        
        // Get the total number of seconds running...
        size_t total_seconds = cronometer.diffTimeInSeconds();

        // Compute the number of lines per second, so far...
        if( total_seconds > 0 )
            lines_per_second = (double)num_lines/(double)total_seconds; 
        
        
        if( ( total_seconds - last_message_time ) > 5 )
        {
            last_message_time = total_seconds;
            LM_V(( "Generated %s lines ( %s bytes ) in %s. Rate: %s / %s", 
                  au::str(num_lines).c_str() , au::str(total_size).c_str(), au::str_time( total_seconds ).c_str() ,
                  au::str( (double)num_lines/(double)total_seconds ,"Lines/s" ).c_str() , au::str( (double)total_size/(double)total_seconds,"Bps").c_str() ));
        }
        
        if( total_seconds > 0 )
            if( max_num_lines > 100 )
                if( (num_lines%( max_num_lines/100)) == 0 )
                {
                    LM_V(( "Generated %s - %s lines ( %s bytes ) in %s. Rate: %s / %s", 
                          au::str_percentage( num_lines,  max_num_lines).c_str(), 
                          au::str(num_lines).c_str() , au::str(total_size).c_str(), au::str_time( total_seconds ).c_str() ,
                          au::str( (double)num_lines/(double)total_seconds ,"Lines/s" ).c_str() , au::str( (double)total_size/(double)total_seconds,"Bps").c_str() ));
                }
      
        
        // Sleep if necessary
        if( max_rate > 0 )
        {
            size_t theoretical_seconds = num_lines / max_rate;
            
            if( total_seconds < theoretical_seconds )
                sleep( theoretical_seconds - total_seconds );
        }
        
    }
    
    buffer_to_screen.flush();
    
    size_t total_seconds = cronometer.diffTimeInSeconds();
    LM_V(( "Generated %s lines ( %s bytes ) in %s. Rate: %s / %s", 
          au::str(num_lines).c_str() , au::str(total_size).c_str(), au::str_time( total_seconds ).c_str() ,
          au::str( (double)num_lines/(double)total_seconds ,"Lines/s" ).c_str() , au::str( (double)total_size/(double)total_seconds,"Bps").c_str() ));

    
}
