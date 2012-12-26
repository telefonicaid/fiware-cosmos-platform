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

#include "logMsg/logMsg.h"
#include "split.h" // Own interface


namespace au 
{
    std::vector<std::string> split_using_multiple_separators( std::string input , std::string separators )
    {
        std::vector<std::string> components;
        size_t pos = 0;
        
        
        while( pos < input.length() )
        {
            size_t s = input.find_first_of( separators , pos );
            
            
            if( s == std::string::npos )
            {
                components.push_back( input.substr( pos , input.length() - pos ) );
                break;
            }
            else
            {
                components.push_back( input.substr( pos , s - pos ) );
                pos=s+1;
            }
        }
        
        
        // Return found components
        return components;
    }
}