
#include "Log.h"


namespace au {
    
    const char*log_reseved_words[] = {"HOST","TYPE","PID","TID","DATE","date","TIME","time","timestamp","LINE","TLEV","EXEC","AUX","FILE","TEXT","text","FUNC","STRE","time_unix","channel",NULL};

    
    void Log::add_field( std::string field_name , std::string field_value )
    {
        if( fields.find(field_name) == fields.end() )
            fields.insert( std::pair<std::string, std::string>( field_name , field_value ));
    }
    
    bool Log::read( au::FileDescriptor *fd )
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
        au::TemporalBuffer buffer( string_length );
        //LM_V(("Reading strings data of %lu bytes" ,string_length ));
        fd->partRead(buffer.data, string_length , "log_strings", 100 );
        
        // Process strings 
        addStrings( buffer.data , string_length );
        return true;
    }
    
    bool Log::write( au::FileDescriptor *fd )
    {
        //LM_V(("Writing %s" , str().c_str() ));
        LogHeader header;
        header.setMagicNumber();
        size_t strings_size = getStringsSize();
        header.dataLen = sizeof(LogData) + strings_size;
        
        // Write header
        au::Status s = fd->partWrite(&header, sizeof(LogHeader), "log header",1,1,0);
        if( s != au::OK )
            return false; // Just quit
        
        // Write data
        s = fd->partWrite(&log_data, sizeof(LogData), "log data",1,1,0);
        if( s != au::OK )
            return false; // Just quit
        
        if( strings_size > 0 )
        {
            TemporalBuffer buffer( strings_size );
            copyStrings( buffer.data );
            
            s = fd->partWrite(buffer.data, strings_size , "log_strings",1,1,0);
            if( s != au::OK )
                return false; // Just quit
        }
        
        return true;        
    }
    
    std::string Log::str()
    {
        std::ostringstream output;
        
        output << "LineNo:" << log_data.lineNo << ",";;
        output << au::str("TraceLevel:%d",log_data.traceLevel) << ",";
        output << "Type:" << log_data.type << ",";
        output << "Time:" << log_data.tv.tv_sec << "(" << log_data.tv.tv_usec << "),";
        output << "TimeZone:" << log_data.timezone << ",";
        output << "Dst:" << log_data.dst << ",";
        
        output << "[ ";
        std::map<std::string, std::string>::iterator it_fields;
        for ( it_fields = fields.begin() ; it_fields != fields.end() ; it_fields++ )
            output << it_fields->first << ":" << it_fields->second << " ";
        output << "}";
        
        return output.str();
    }
    
    
    // Get information from this log
    std::string Log::get( std::string  name )
    {
        LM_V(("Getting %s from log %s" , name.c_str() , str().c_str() ));
        
        if( name == "HOST" )
            return getField( "host" , "" ); 
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
            return std::string( line_tmp );
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
        
        if( name == "timestamp" )
        {
            return get("date") + " " + get("time");
        }
        
        if( name == "time_unix" )
        {
            return au::str("%lu" , log_data.tv.tv_sec );
        }
        
        if( name == "TIME" )
        {
            struct tm timeinfo;
            char buffer_time[1024];
            localtime_r ( &log_data.tv.tv_sec , &timeinfo );
            strftime (buffer_time,1024,"%X",&timeinfo);
            return std::string( buffer_time ) + au::str("(%d)" , log_data.tv.tv_usec ) ;
        }
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
        
        // Generl look up in the strings...
        std::map<std::string, std::string>::iterator it_fields = fields.find(name);
        if( it_fields != fields.end() )
            return it_fields->second;
        
        // If not recognized as a field, just return the name
        return name;
    }
    
    
    
    std::string Log::getField( std::string name , std::string default_value )
    {
        std::map<std::string, std::string>::iterator it_fields = fields.find(name);
        if( it_fields == fields.end() )
            return default_value;
        else
            return it_fields->second;
    }
    
    size_t Log::getTotalSerialitzationSize()
    {
        return sizeof(LogHeader) + sizeof(LogMsg) + getStringsSize();
    }
    
    size_t Log::getStringsSize()
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
    
    void Log::copyStrings( char * data )
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
    
    
    void Log::addStrings( char* strings , size_t len )
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

    void Log::set_new_session()
    {
        log_data.lineNo=0;
        log_data.traceLevel=0;
        log_data.type='S';
        log_data.tv.tv_sec = time(NULL); 
        log_data.tv.tv_usec = 0;
        log_data.timezone=0;
        log_data.dst=0;
        log_data.pid=0;
        
        add_field("new_session", "yes");
    }
    
    bool Log::is_new_session()
    {
        return getField("new_session", "no") == "yes";
    }
    

    
}
