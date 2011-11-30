#ifndef _AU_CONSOLE_COMMAND_HISTORY
#define _AU_CONSOLE_COMMAND_HISTORY

#include <vector>
#include <string>

#include "au/au_namespace.h"

NAMESPACE_BEGIN(au)

class ConsoleCommand;

class ConsoleCommandHistory
{
    std::vector<ConsoleCommand*> commands;
    size_t pos;
    
    std::string file_name;
    
public:
    
    ConsoleCommandHistory();
    ~ConsoleCommandHistory();
    
    ConsoleCommand* current();
    
    
    void recover_history( );
    void save_history();
    
    void move_up();
    void move_down();
    void new_command();
    
};

NAMESPACE_END

#endif