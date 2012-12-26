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

#ifndef _H_SAMSON_CONNECTOR_LOG_MANAGER
#define _H_SAMSON_CONNECTOR_LOG_MANAGER

#include "au/tables/Table.h"
#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"
#include "engine/BufferContainer.h"


namespace stream_connector {
    
    // Log system for channel - item - connection
    
    class Log
    {
        time_t time_;
        std::string type_;
        std::string name_;
        std::string message_;
        
        friend class LogManager;
        
        
    public:
        
        Log( std::string name , std::string type , std::string message );
        
        std::string getNameAndMessage();
        std::string getType();
        void writeOnScreen();
        
    };
    
    
    class LogManager
    {
        au::Token token;
        au::list<Log> logs;
        
    public:
        
        
        LogManager();        
        
        void log( Log* l );
        au::tables::Table* getLogsTable( size_t limit );
        
    };
        
} // End of namespace stream_connector
    
#endif
