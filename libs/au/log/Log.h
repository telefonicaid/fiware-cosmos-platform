
#ifndef _H_AU_LOG
#define _H_AU_LOG

#define LOG_SERVER_DEFAULT_CHANNEL_PORT            6001
#define LOG_SERVER_DEFAULT_CHANNEL_NAME            "default"

#define DATE_FORMAT "%A %d %h %H:%M:%S %Y"

#include <fcntl.h>

#include "au/containers/vector.h"
#include "au/Status.h"
#include "au/StringVector.h"
#include "au/network/FileDescriptor.h"
#include "au/Tokenizer.h"
#include "au/string/split.h"
#include "au/tables/Table.h"

namespace au {
    
    class TemporalBuffer
    {
        
    public:
        
        char *data;
        
        TemporalBuffer( size_t size )
        {
            data = (char*) malloc(size);
        }
        
        ~TemporalBuffer()
        {
            if(data)
                free (data);
        }
        
        
    };
    
    
    /*
     
     Known string fields
     ---------------------------------------------------------------------------
     "progName";
     "text";
     "file";
     "fName";
     "stre";
     
     Formats:
     ---------------------------------------------------------------------------
     
     TYPE     ( in LogData )
     PID      ( in LogData ) 
     DATE     ( in LogData )
     date     ( in LogData )
     TIME     ( in LogData )
     time     ( in LogData )
     TID      ( in LogData )
     LINE     ( in LogData )
     TLEV     ( in LogData )
     EXEC     progName     // Name of the executable ( progname )
     AUX      aux          // aux variable to distinguish fork files...
     FILE     file         // Name of the source file
     TEXT     text         // Message
     text     text         // Message limited to 60 chars
     FUNC     fname        // Function name
     STR      stre         // ?
     
     */
    
    // Entry in the log
    class Log
    {
        
    public:
        
        
        // ---------------------------------------------
        // Information about this trace
        // ---------------------------------------------
        LogData log_data;  
        std::map<std::string, std::string> fields;
        // ---------------------------------------------
        
        Log( )
        {
        }
        
        ~Log()
        {
        }
        
        void add_field( std::string field_name , std::string field_value )
        {
            if( fields.find(field_name) == fields.end() )
                fields.insert( std::pair<std::string, std::string>( field_name , field_value ));
        }
        
        bool read( au::FileDescriptor *fd )
        {
            
            //LM_V(("Reading log header of %lu bytes" ,sizeof(LogHeader)));
            LogHeader header;
            au::Status s = fd->partRead(&header, sizeof(LogHeader), "log header", 300);
            
            if( s != au::OK )
                return false; // Just quit
            
            if( !header.checkMagicNumber() )
            {
                LM_E(("Wrong log header.Expected magic number %d but received %d. Closing connection..."
                      , _LM_MAGIC , header.magic ));
                return false;
            }
            
            // Read fixed data
            //LM_V(("Reading fix log data of %lu bytes" ,sizeof(LogData)));
            s = fd->partRead(&log_data, sizeof(LogData), "log data", 300);
            
            if( s != au::OK )
                return false; // Just quit
            
            // Read strings
            size_t string_length = header.dataLen - sizeof(LogData);
            TemporalBuffer buffer( string_length );
            //LM_V(("Reading strings data of %lu bytes" ,string_length ));
            fd->partRead(buffer.data, string_length , "log_strings", 100 );
            
            // Process strings 
            addStrings( buffer.data , string_length );
            return true;
        }
        
        bool write( au::FileDescriptor *fd )
        {
            //LM_V(("Writing %s" , str().c_str() ));
            LogHeader header;
            header.setMagicNumber();
            size_t strings_size = getStringsSize();
            header.dataLen = sizeof(LogData) + strings_size;
            
            // Write header
            au::Status s = fd->partWrite(&header, sizeof(LogHeader), "log header");
            if( s != au::OK )
                return false; // Just quit
            
            // Write data
            s = fd->partWrite(&log_data, sizeof(LogData), "log data");
            if( s != au::OK )
                return false; // Just quit
            
            if( strings_size > 0 )
            {
                TemporalBuffer buffer( strings_size );
                copyStrings( buffer.data );
                
                s = fd->partWrite(buffer.data, strings_size , "log_strings");
                if( s != au::OK )
                    return false; // Just quit
            }
            
            return true;        
        }
        
        std::string str()
        {
            std::ostringstream output;
            
            output << "LineNo:" << log_data.lineNo << ",";;
            output << au::str("TraceLevel:%d",log_data.traceLevel) << ",";
            output << "Type:" << log_data.type << ",";
            output << "Time:" << log_data.tv.tv_sec << "(" << log_data.tv.tv_usec << "),";
            output << "TimeZone:" << log_data.timezone << ",";
            output << "Dst:" << log_data.dst << ",";
            
            std::map<std::string, std::string>::iterator it_fields;
            for ( it_fields = fields.begin() ; it_fields != fields.end() ; it_fields++ )
                output << it_fields->first << ":" << it_fields->second;
            
            return output.str();
        }
        
        
        // Get information from this log
        std::string get( std::string  name )
        {
            
            if( name == "TYPE" )
                return au::str("%c" , log_data.type ); 
            if( name == "PID" )
                return au::str("%d" , log_data.pid ); 
            if( name == "TID" )
                return au::str("%d" , log_data.tid ); 
            if( name == "DATE" )
            {
                char line_tmp[80];
                struct tm tmP;
                gmtime_r(&log_data.tv.tv_sec, &tmP);
                strftime(line_tmp, 80, DATE_FORMAT, &tmP);
                return std::string( line_tmp ) + au::str("(%d)" , log_data.tv.tv_usec );
            }
            if( name == "date" )
            {
                struct tm timeinfo;
                char buffer_time[1024];
                localtime_r ( &log_data.tv.tv_sec , &timeinfo );
                strftime (buffer_time,1024,"%d/%m/%Y",&timeinfo);
                return std::string( buffer_time );
            }
            if( name == "time" )
            {
                struct tm timeinfo;
                char buffer_time[1024];
                localtime_r ( &log_data.tv.tv_sec , &timeinfo );
                strftime (buffer_time,1024,"%X",&timeinfo);
                return std::string( buffer_time );
            }
            if( name == "TIME" )
                return au::str_timestamp( log_data.tv.tv_sec ); 
            if( name == "LINE" )
                return au::str("%d" , log_data.lineNo );
            if( name == "TLEV" )
                return au::str("%d" , log_data.traceLevel );
            if( name == "EXEC" )
                return getField( "progName" , "" ); 
            if( name == "AUX" )
                return getField( "aux" , "" ); 
            if( name == "FILE" )
                return getField( "file" , "" ); 
            if( name == "TEXT" )
                return getField( "text" , "" ); 
            if( name == "text" )
            {
                std::string t = getField( "text" , "" );
                if( t.length() > 80 )
                    return t.substr(0,80);
                else
                    return t;
            }
            if( name == "FUNC" )
                return getField( "fname" , "" ); 
            if( name == "STRE" )
                return getField( "stre" , "" ); 
            
            
            // If not recognized as a field, just return the name
            return name;
        }
        
        
        
        std::string getField( std::string name , std::string default_value )
        {
            std::map<std::string, std::string>::iterator it_fields = fields.find(name);
            if( it_fields == fields.end() )
                return default_value;
            else
                return it_fields->second;
        }
        
        size_t getTotalSerialitzationSize()
        {
            return sizeof(LogHeader) + sizeof(LogMsg) + getStringsSize();
        }
        
    private:
        
        size_t getStringsSize()
        {
            size_t total = 0;
            std::map<std::string, std::string>::iterator it_fields;
            for ( it_fields = fields.begin() ; it_fields != fields.end() ; it_fields++ )
            {
                total += it_fields->first.length();
                total++;
                
                total += it_fields->second.length();
                total++;
            }
            
            return total;
        }
        
        void copyStrings( char * data )
        {
            size_t pos = 0;
            std::map<std::string, std::string>::iterator it_fields;
            for ( it_fields = fields.begin() ; it_fields != fields.end() ; it_fields++ )
            {
                memcpy(&data[pos], it_fields->first.c_str(), it_fields->first.length()+1 );
                pos += it_fields->first.length();
                pos++;
                
                memcpy(&data[pos], it_fields->second.c_str(), it_fields->second.length()+1 );
                pos += it_fields->second.length();
                pos++;
                
            }
            
        }
        
        
        void addStrings( char* strings , size_t len )
        {
            std::vector<std::string> vector_strings;
            
            size_t pos = 0;
            for ( size_t i = 0 ; i < len ; i++ )
                if( strings[i] == '\0' )
                {
                    vector_strings.push_back( &strings[pos] );
                    pos = i+1;
                }
            
            //LM_V(("Readed %lu strins" , vector_strings.size() ));
            
            // Add to the map
            for ( size_t i = 0 ; i < vector_strings.size()/2 ; i++ )
            {
                std::string field_name = vector_strings[2*i];
                std::string field_value = vector_strings[2*i+1];
                
                //LM_V(("Added %s=%s" , field_name.c_str() , field_value.c_str() ));
                
                // Add field
                add_field( field_name , field_value );
            }
            
        }
        
    };
    
    
    
    class LogFile
    {        
        
    public:
        
        // Collections of logs from this file
        au::vector<Log> logs;
        
        // Read a file
        static au::Status read( std::string file_name , LogFile** logFile )
        {
            int fd = open( file_name.c_str() , O_RDONLY );
            if( fd < 0 )
                return OpenError;
            
            // File descriptor to read logs
            FileDescriptor file_descriptor( "reading log file" , fd );
            
            // Create log file
            *logFile = new LogFile();
            
            while (true) 
            {
                Log* log = new Log();
                if( log->read(&file_descriptor) )
                    (*logFile)->logs.push_back( log );
                else
                    break;
            }
            
            
            if( (*logFile)->logs.size() == 0 )
            {
                delete *logFile;
                *logFile = NULL;
                return Error;
            }
            
            return OK;
        }
        
        
    };
    
    class LogFormatter
    {
        
        std::vector<std::string> fields;
        
    public:
        
        LogFormatter( std::string definition )
        {
            //LM_V(("Log formatter defined with '%s'" , definition.c_str() ));
            
            au::token::TokenVector token_vector;
            token_vector.addToken("TYPE");
            token_vector.addToken("PID");
            token_vector.addToken("TID");
            token_vector.addToken("DATE");
            token_vector.addToken("date");
            token_vector.addToken("TIME");
            token_vector.addToken("time");
            token_vector.addToken("LINE");
            token_vector.addToken("TLEV");
            token_vector.addToken("EXEC");
            token_vector.addToken("AUX");
            token_vector.addToken("FILE");
            token_vector.addToken("TEXT");
            token_vector.addToken("text");
            token_vector.addToken("FUNC");
            token_vector.addToken("STRE");
            
            token_vector.parse( definition );
            
            
            au::token::Token* token = token_vector.getNextToken();
            while( token )
            {
                fields.push_back( token->content );
                token_vector.popToken();
                token = token_vector.getNextToken();
            }
            
            //LM_V(("Log formatter with %lu tokens" , fields.size() ));
            
        }
        
        std::string get( Log* log )
        {
            std::string output;
            for( size_t i = 0 ; i < fields.size() ; i++ )
                output.append(  log->get( fields[i] ) );
            return output;
        }
        
        
    };
    
    class TableLogFormatter
    {
        
        // Table formatted
        std::vector<std::string> table_fields;
        au::tables::Table* table;
        
        // Formatter for logs
        LogFormatter* log_formatter;
        std::ostringstream output;
        
    public:
        
        TableLogFormatter( bool is_table , std::string format )
        {
            if( is_table )
            {
                // Table fields
                table_fields = split_using_multiple_separators( format , "|:;,-/\[] ()." );
                
                std::string table_definition;
                for ( size_t i = 0 ; i < table_fields.size() ; i++ )
                    table_definition.append(  au::str("%s,left|", table_fields[i].c_str() ) );
                
                table = new au::tables::Table( table_definition );
                log_formatter = NULL;
                
            }
            else
            {
                log_formatter = new LogFormatter( format );
                table = NULL;
            }
            
            
        }
        
        ~TableLogFormatter()
        {
            if( table )
                delete table;
            if( log_formatter )
                delete log_formatter;
        }
        
        void add( Log* log )
        {
            if( table )
            {
                StringVector values;
                for ( size_t i = 0 ; i < table_fields.size() ; i++ )
                    values.push_back( log->get(table_fields[i]) );
                table->addRow( values );
            }
            else
                output << log_formatter->get( log ) << "\n";
            
            
        }
        
        std::string str()
        {
            if( table )
                return table->str();
            else
                return output.str();
            
        }
        
    };
    
}

#endif