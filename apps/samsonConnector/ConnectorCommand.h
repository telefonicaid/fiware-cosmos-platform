
#ifndef _H_SAMSON_CONNECTOR_COMMAND
#define _H_SAMSON_CONNECTOR_COMMAND

namespace samson 
{
    namespace connector
    {
        
        /*
         
         SamsonConnectorCommandLine
         
         Command for a samsonConnector instance
         It is passes acros multiple elements Channel, Item, etc...
         
         */
        
        class CommandLine : public au::CommandLine
        {
            
        public:
            
            CommandLine( std::string command )
            {
                set_flag_string("channel", "default");
                parse( command );
            }
            
            std::string getChannel()
            {
                return get_flag_string("channel");
            }
            
        };
    }
}

#endif
