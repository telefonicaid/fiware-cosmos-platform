/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

#ifndef _H_SAMSON_CONNECTOR_COMMAND
#define _H_SAMSON_CONNECTOR_COMMAND

namespace stream_connector {
    
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
            set_flag_boolean("data");
            parse( command );
        }
        
        bool isDataFlag()
        {
            return get_flag_bool("data");
        }
        
        
    };
}

#endif
