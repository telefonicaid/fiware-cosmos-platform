
#ifndef _AU_CONSOLE
#define _AU_CONSOLE

#include <string>
#include <list>
#include <termios.h>                // termios

#include "au/ConsoleCode.h"
#include "au/Token.h"

#include "au/au_namespace.h"

NAMESPACE_BEGIN(au)

class Console;
class ConsoleAutoComplete;
class ConsoleCommandHistory;

class Console
{
    ConsoleCommandHistory *command_history;
    
    // Flag to quit internal loop
    bool quit_console;
    
    // Pending messages to be displayed
    pthread_t t_running;
    std::list< std::string > pending_messages;
    au::Token token_pending_messages;
    
    int counter; // Used only for testing
    
public:
    
    Console();
    ~Console();
    
    void runConsole();
    void quitConsole();
    
    /* Methods to write things on screen */
    void writeWarningOnConsole( std::string message );
    void writeErrorOnConsole( std::string message );
    void writeOnConsole( std::string message );
    
    // Customize console
    virtual std::string getPrompt();
    virtual void evalCommand( std::string command );
    virtual void autoComplete( ConsoleAutoComplete* info );
    
    void refresh();
    
    // Make sure all messages are shown
    void flush();
    
private:
    
    void print_command();
    bool isImputReady();
    
    
    void process_auto_complete( ConsoleAutoComplete * info );
    void process_char( char c );
    void process_code( ConsoleCode code );
    
    void process_background();
    
    bool isNormalChar( char c );
    
    
    void write( std::string message );
    
};

NAMESPACE_END

#endif

