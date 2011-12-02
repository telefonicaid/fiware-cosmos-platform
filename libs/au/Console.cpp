#include <iostream>
#include <sstream>
#include <sys/ioctl.h>
#include <stdio.h>
#include <signal.h>
#include "au/string.h"
#include "au/list.h"
#include "au/Cronometer.h"
#include "au/utils.h"

#include "au/TokenTaker.h"

#include "au/ConsoleAutoComplete.h"
#include "au/ConsoleCommand.h"
#include "au/ConsoleCommandHistory.h"
#include "au/ConsoleCode.h"
#include "au/ConsoleEscapeSequence.h"


#include "au/Console.h"  // Own interface


NAMESPACE_BEGIN(au)

Console *current_console=NULL;
void handle_winch(int sig);

Console::Console() : token_pending_messages("token_pending_messages")
{
    command_history = new ConsoleCommandHistory();
    counter = 0;
}

Console::~Console()
{
    delete command_history;
}

void Console::init()
{
    /* get the terminal settings for stdin */
    tcgetattr(STDIN_FILENO,&old_tio);
    
    /* we want to keep the old setting to restore them a the end */
    new_tio=old_tio;
    
    /* disable canonical mode (buffered i/o) and local echo */
    new_tio.c_lflag &=(~ICANON & ~ECHO);
    
    /* set the new settings immediately */
    tcsetattr(STDIN_FILENO,TCSANOW,&new_tio);
    
}

void Console::finish()
{
    /* set the new settings immediately */
    tcsetattr(STDIN_FILENO,TCSANOW,&old_tio);
}

void Console::print_command()
{
    std::string _command = command_history->current()->getCommand();
    int _pos = command_history->current()->getPos();
    
    clear_line();
    //printf("[%d]",counter); // debuggin
    
    printf("%s %s", getPrompt().c_str() , _command.c_str());
    
    if( _pos != (int)_command.length() )
    {
        printf("\r");
        
        //printf("[%d]",counter); // debuggin
        
        printf("%s " ,  getPrompt().c_str() );
        for (int i = 0 ; i < _pos ; i++ )
            printf("%c",_command[i]);
    }
    
    counter++;
    
    fflush(stdout);
    // At the moment we are not considering pos_command
}


bool Console::isImputReady()
{
    struct timeval  timeVal;
    timeVal.tv_sec  = 0;
    timeVal.tv_usec = 0;
    
    fd_set          rFds;
    FD_ZERO(&rFds);
    FD_SET(0, &rFds);
    
    int s = select( 1, &rFds, NULL, NULL, &timeVal);
    
    return (s==1);
}


void Console::process_auto_complete( ConsoleAutoComplete * info )
{
    // Do something with autocomplete information provided by thrid party
    //print_command();
    //printf("\n");

    clear_line();
    
    info->print_last_words_alternatives();
    
    // Add necessary stuff...    
    command_history->current()->add( info->stringToAppend() );
    print_command();
    
}

void Console::process_char( char c )
{
    command_history->current()->add(c);
    
    if ( command_history->current()->isCursorAtEnd() )
    {
        printf("%c",c);
        fflush(stdout);
    }
    else
        print_command();
}


void Console::process_code( ConsoleCode code )
{
    switch (code) {
        case ret:
        {
            std::string _command = command_history->current()->getCommand();
            
            //Print the command on screen...
            print_command();
            printf("\n");
            
            // Eval the command....
            evalCommand( _command );
            
            //New command in history
            command_history->new_command();
            print_command();
        }
            break;
        case tab:
        {
            ConsoleAutoComplete* info = new ConsoleAutoComplete( command_history->current()->getCommandUntilPointer() );
            autoComplete( info );
            process_auto_complete( info );
            delete info;
            
        }
            break;
        case del:
            command_history->current()->delete_char();
            print_command();
            break;
        case del_word:
            command_history->current()->delete_word();
            print_command();
            break;
        case move_forward:
            command_history->current()->move_cursor(1);
            print_command();
            break;
        case move_backward:
            command_history->current()->move_cursor(-1);
            print_command();
            break;
        case move_up:
            command_history->move_up();
            print_command();
            break;
        case move_down:
            command_history->move_down();
            print_command();
            break;
        case move_home:
            command_history->current()->move_home();
            print_command();
            break;
        case move_end:
            command_history->current()->move_end();
            print_command();
            break;
        case del_rest_line:
            command_history->current()->delete_rest_line();
            print_command();
            break;
        case au:
            command_history->current()->add("Andreu Urruela (andreu@tid.es,andreu@urruela.com)");
            print_command();
            break;
            
        default:
            break;
    }
    /*            
     printf("[%s]", getConsoleCodeName(code) );
     fflush(stdout);
     */
}

void Console::process_background()
{
    if ( pending_messages.size() != 0 )
    {
        au::TokenTaker tt(&token_pending_messages);
        
        clear_line();
        
        while (pending_messages.size() != 0) 
        {
            std::string txt = pending_messages.front();
            pending_messages.pop_front();
            printf("%s", txt.c_str() );
        }
        
        printf("\n\n");
        print_command();
        fflush(stdout);
        
    }
}

void Console::flush()
{
    process_background();
    
}


bool Console::isNormalChar( char c )
{
    if( ( c>= 32) && ( c<=126))
        return true;
    return false;
}

void Console::runConsole()
{
    // Init console
    init();
    
    t_running = pthread_self();
    
    // Signal to handle terminal changes...
    current_console = this;
    signal(SIGWINCH, handle_winch);
    
    // First version with just the promtp
    print_command();
    
    // Escape sequence... 
    ConsoleEscapeSequence escape_sequence;
    bool escaping = false;   // Fag to indicate if we are inside a scape sequence...
    
    quit_console = false;
    while( !quit_console )
    {
        
        while ( !isImputReady() )
        {
            // Review background messages
            process_background();
            usleep(20000);
        }
        
        char c;
        if( read( 0 , &c , 1 ) != 1 )
            LM_X(1, ("reading from stdin failed"));
        
        if( escaping )
        {
            escape_sequence.add( c );
            
            ConsoleCode code = escape_sequence.getCode();
            
            
            if ( code != unfinished )
            {
                if ( code == unknown )
                    writeWarningOnConsole( au::str( " Unknown escape sequence (%s)" , escape_sequence.description().c_str() ).c_str() );
                else
                    process_code(code);
                escaping = false;
            }
        }
        else
        {
            if ( isNormalChar(c) )
                process_char(c);
            else if ( c == '\n' )
                process_code( ret );
            else if ( c == '\t' )
                process_code( tab );
            else if ( c == 127 )
                process_code( del );
            else if ( c == 11 )
                process_code( del_rest_line );
            else if ( c == 1 )
                process_code( move_home );
            else if ( c == 5 )
                process_code( move_end );
            else if ( c == 7 ) // bell
                printf("%c",c);
            else if ( c == 27 )
            {
                escaping = true;  // Start a scape mode
                escape_sequence.init();
            }
            else
            {
                writeWarningOnConsole( au::str( " Ignoring char (%d)" , c ) );                        
            }
            
        }
        
        
    }
    
    finish();
    
    // Remove the signal handler...
    signal(SIGWINCH, SIG_IGN);
    
}

void Console::quitConsole()
{
    quit_console = true;
}


/* Methods to write things on screen */
void Console::writeWarningOnConsole( std::string message )
{
    std::ostringstream output;
    output << "\033[1;35m"<< message << "\033[0m";
    write( output.str() );
}


void Console::writeErrorOnConsole( std::string message )
{
    std::ostringstream output;
    output << "\033[1;31m"<< message << "\033[0m";
    write( output.str() );
}


void Console::writeOnConsole( std::string message )
{
    Console::write( message );
}


void Console::write( std::string message )
{
    if( pthread_self() != t_running )
    {
        // Accumulate message
        au::TokenTaker tt(&token_pending_messages);
        pending_messages.push_back( message );
        return;
    }
    
    clear_line();
    printf("%s\n", message.c_str() );
    print_command();
    fflush( stdout );
}



std::string Console::getPrompt()
{
    return "> ";
}


void Console::evalCommand( std::string command )
{        
    writeWarningOnConsole("Console::evalCommand not implemented");
    
    // Simple quit function...
    if ( command == "quit" )
        quit_console = true;
}


void Console::autoComplete( ConsoleAutoComplete* info )
{
    writeWarningOnConsole("Console::auto_complete not implemented");
}

void Console::refresh()
{
    print_command();
}


void handle_winch(int sig)
{
    if( !current_console )
        return;
    
    // Rewrite current command
    current_console->refresh();
}

NAMESPACE_END