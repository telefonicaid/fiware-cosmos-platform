

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

#include "au/string.h" // au::str()
#include "au/Cronometer.h"  // au::Cronometer
#include "au/CommandLine.h" // au::CommandLine


class Position
{
    
public:
    
    double x;
    double y;
    
    Position()
    {
        x = 0;
        y = 0;
    }
    
    Position( double _x , double _y )
    {
        x = _x;
        y = _y;
    }

    void set( double _x , double _y )
    {
        x = _x;
        y = _y;
    }
    
    void set_in_limits( )
    {
        set_in_limits(&x);
        set_in_limits(&y);
    }    
    
private:
    
    void set_in_limits( double* var )
    {
        if( *var <  0)
            *var = 0;
        if( *var > 1000 )
            *var = 1000;
    }    

};

Position getHome( size_t user )
{
    return Position( 10 * ( user%100 )  , 1000 - 10 * ( user%100 ) );
}

Position getWork( size_t user )
{
    return Position( 1000 - 10 * ( user%100 ) , 10 * ( user%100 )  );
}

int period;

Position getPosition( size_t user )
{
    if( time(NULL)%period < (period/2) )
        return getHome(user);
    else
        return getWork(user);
    
    return Position( rand()%1000 , rand()%1000 );
}

unsigned long int  num_users;
size_t current_user;
bool progresive;

size_t getUser()
{
    if ( progresive )
    {
        current_user++;
        if( current_user >= num_users )
            current_user=0;
        return current_user;
    }
    
    return rand()%num_users;
}

int main( int args , const char*argv[] )
{
    // Random sequence generated
    srand( time(NULL));
    
    au::CommandLine cmd;
    cmd.set_flag_uint64("users", 40000000 );    // Number of users
    cmd.set_flag_uint64("rate", 10000 );    // Number of CDRS per second
    cmd.set_flag_uint64("period", 300 );    // Period of work-home in seconds
    cmd.set_flag_boolean("h");
    cmd.set_flag_boolean("help");
    cmd.set_flag_boolean("commands");
    cmd.set_flag_boolean("progressive");
    
    cmd.parse( args, argv );

    period = cmd.get_flag_uint64("period");
    progresive = cmd.get_flag_bool("progressive");
    num_users = cmd.get_flag_uint64("users");
    current_user = 0;

    if( cmd.get_flag_bool("h") || cmd.get_flag_bool("help") )
    {
        printf("\n");
        printf(" ------------------------------------------------- \n");
        printf(" Help %s\n" , argv[0] );
        printf(" ------------------------------------------------- \n");
        printf(" Simple command line tool to generate fake data for simple_mobility demo\n\n");
        printf(" %s -commands       Generates the command to setup home/work areas\n", argv[0] );
        printf(" %s                 Generates the CDRS \n", argv[0] );
        printf("\n");
        printf(" Option: -users  X     Change the number of users ( default 20000000 ) \n" );
        printf(" Option: -rate   X     Change number of CDRS per second ( default 10000 ) \n" );
        printf(" Option: -period X     Change period work-home in seconds (default 300 secs )\n");
        printf(" Option: -progressive  Non random sequence of messages\n");
        printf("\n");
        return 0;
    }
    
    
    unsigned long int rate = cmd.get_flag_uint64("rate");
    
    fprintf(stderr,"%s: Setup %lu users and %lu cdrs/second\n" , argv[0] , num_users , rate );
    
    size_t total_num = 0;
    size_t total_size = 0;
    
    if( cmd.get_flag_bool("commands") )
    {
        for ( unsigned long int i = 0 ; i < num_users ; i++ )
        {
            Position home = getHome(i);
            Position work = getWork(i);
            printf("%lu AREA_CREATE home %f %f 200 \n" , i ,  home.x , home.y );
            printf("%lu AREA_CREATE work %f %f 200 \n" , i , work.x , work.y );
        }
        
        fprintf(stderr,"%s: Generated %lu messages" , argv[0] , num_users );
        return 0;
    }
    
    
    au::Cronometer cronometer;
    size_t theoretical_seconds = 0;
    
    while( true )
    {
        // Generate messages for the next second....
        theoretical_seconds += 1;         
        for ( unsigned long int i = 0 ; i < rate ; i++ )
        {
            unsigned long int user = getUser();
            
            Position p = getPosition( user );
            
            total_size += printf("%lu CDR %f %f %lu\n", user , p.x , p.y , time(NULL) );
            total_num++;
        }
        
        size_t total_seconds = cronometer.diffTimeInSeconds();
        
        if( total_seconds < theoretical_seconds )
        {
            int seconds_to_sleep = (int) theoretical_seconds - total_seconds;
            fprintf(stderr,"%s: Sleeping %d seconds to keep rate %s\n", 
                    argv[0] , seconds_to_sleep , au::str( rate , "Events/sec" ).c_str() );
            sleep( seconds_to_sleep );
        }
        
        if( (theoretical_seconds%10) == 0)
        {
            fprintf(stderr,"%s: Generated %s lines ( %s bytes ) in %s. Rate: %s / %s.\n" 
                    , argv[0] , au::str(total_num).c_str() , au::str(total_size).c_str(), au::time_string( total_seconds ).c_str() ,
                    au::str( (double)total_num/(double)total_seconds ,"Lines/s" ).c_str() , au::str( (double)total_size/(double)total_seconds,"Bps").c_str()  );
        }
        
        
    }
    
}
