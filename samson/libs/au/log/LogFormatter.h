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


#ifndef _H_AU_LOG_FORMATTER
#define _H_AU_LOG_FORMATTER


#define DEFAULT_FORMAT "TYPE : date : time : EXEC : FILE[LINE] : FUNC : TEXT"

namespace au 
{
    
    class LogFormatter
    {
        
        std::string definition;
        std::vector<std::string> fields;
        
    public:
        
        LogFormatter( std::string _definition = DEFAULT_FORMAT );        
        std::string get( Log* log );
        
    };
}

#endif