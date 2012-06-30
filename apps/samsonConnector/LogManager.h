
#ifndef _H_SAMSON_CONNECTOR_LOG_MANAGER
#define _H_SAMSON_CONNECTOR_LOG_MANAGER

#include "au/tables/Table.h"
#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"
#include "engine/BufferContainer.h"


namespace samson { namespace connector {
    
    // Log system for channel - item - connection
    
    class Log
    {
        time_t time_;
        std::string type_;
        std::string name_;
        std::string message_;
        
        friend class LogManager;
        
        
    public:
        
        Log( std::string name , std::string type , std::string message )
        {
            time_ = time(NULL);
            type_ = type;
            name_ = name;
            message_ = message;
        }
        
        std::string getNameAndMessage()
        {
            return au::str( "%25s : %-s" , name_.c_str(), message_.c_str() );
        }
        
        std::string getType()
        {
            return type_;
        }
        
        void writeOnScreen()
        {
            if( type_ == "Warning" )
                LM_W(( getNameAndMessage().c_str() ));
            else if( type_ == "Error" )
                LM_E(( getNameAndMessage().c_str() ));
            else
                LM_M(( getNameAndMessage().c_str() ));
        }
        
    };
    
    
    class LogManager
    {
        au::Token token;
        au::list<Log> logs;
        
    public:
        
        
        LogManager() : token( "LogManager")
        {
            
        }
        
        void log( Log* l )
        {
            au::TokenTaker tt(&token);
            logs.push_back(l);
            
            while( logs.size() > 1000000 )
            {
                Log* log = logs.front();
                delete log;
                logs.pop_front();
            }
            
        }
        
        au::tables::Table* getLogsTable( size_t limit )
        {
            au::tables::Table* table = new au::tables::Table ( "Element|Time,left|Type,left|Description,left" );
            table->setTitle("Logs");
            
            au::TokenTaker tt(&token);
            au::list<Log>::iterator it;
            
            for( it = logs.begin() ; it != logs.end() ; it++ )
            {
                Log* log = *it;
                au::StringVector values;
                values.push_back( log->name_ );
                
                values.push_back( au::str_timestamp( log->time_ ) );
                
                values.push_back( log->type_ );
                values.push_back( log->message_ );
                
                table->addRow(values);
            }
            
            return table;
        }
        
        
    };
    
    template< class C >
    class Singleton
    {
        static au::Token token;
        static C* instance;
        
    public:
        
        static C* shared()
        {
            au::TokenTaker tt(&token);
            if( !instance )
                instance = new C();
            return instance;
        }
        
    };
    
    // Static members
    template <class C> C* Singleton<C>::instance;
    template <class C> au::Token Singleton<C>::token("singleton");
    
    //LogManager* log_manager = Singleton<LogManager>::shared();
    
    
}} // End of namespace samson connector
    
#endif
