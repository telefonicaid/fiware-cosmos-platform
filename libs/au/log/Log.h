
#ifndef _H_AU_LOG
#define _H_AU_LOG

#define LOG_SERVER_DEFAULT_CHANNEL_PORT            6001
#define LOG_SERVER_DEFAULT_CHANNEL_NAME            "default"

#define DATE_FORMAT "%A %d %h %H:%M:%S %Y"

#include <fcntl.h>

#include "au/containers/vector.h"
#include "au/Status.h"
#include "au/containers/StringVector.h"
#include "au/network/FileDescriptor.h"
#include "au/Tokenizer.h"
#include "au/string/split.h"
#include "au/tables/Table.h"
#include "au/TemporalBuffer.h"

namespace au {
    
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

     HOST     ( part of the logServer information )
     
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
    
    extern const char*log_reseved_words[];
    
    // Entry in the log
    class Log
    {
        
    public:
        
        LogData log_data;  

        std::map<std::string, std::string> fields;
        
        Log( )
        {
        }
        
        ~Log()
        {
        }

        // Set and get methods for string-kind fields
        void add_field( std::string field_name , std::string field_value );
        std::string getField( std::string name , std::string default_value );
        
        // Read and Write over a file descriptor ( network or disk )
        bool read( au::FileDescriptor *fd );
        bool write( au::FileDescriptor *fd );
        
        // Debug string
        std::string str();
        
        // Get information about a particular field
        std::string get( std::string  name );

        // Get total number og bytes when serialized
        size_t getTotalSerialitzationSize();
        
    private:

        // Methods to serialize string-kind fields
        size_t getStringsSize();
        void copyStrings( char * data );
        void addStrings( char* strings , size_t len );
        
    };
}

#endif