


#include "au/string.h"

#include "au/Console.h"
#include "au/ConsoleAutoComplete.h"

class MyConsole : public au::Console
{
  
public:
    virtual std::string getPrompt()
    {
        return "samson >";
    }
    
    void auto_complete( au::ConsoleAutoComplete* info )
    {
        // Fill inside this structure to complete information
        if( info->completingFirstWord() )
        {
            info->add("andreu1");
            info->add("andreu2");
            info->add("pepe");
            info->add("juan");
            
            for (int i =0;i<100;i++)
                info->add( au::str("option_%d",i) );
            
        }
        
        if( info->completingSecondWord( "andreu1" ) )
        {
            info->add("Urruela");
            info->add("Planas");
        }
        
        
    }

    void evalCommand( std::string command )
    {
        if (command == "quit") 
        {
            quitConsole();
        }
    }
    
    
};

int main()
{
   MyConsole console;
   console.runConsole();
}
