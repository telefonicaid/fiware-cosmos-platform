#ifndef _H_AU_TABLE_LOG_FORATTER
#define _H_AU_TABLE_LOG_FORATTER

#include <string>
#include <vector>

#include "au/tables/Table.h"
namespace au 
{
    
    class LogFormatter;
    class Log;
    
    class TableLogFormatter
    {
        
        // Table formatted
        std::vector<std::string> table_fields;
        au::tables::Table* table;
        
        // Formatter for logs
        LogFormatter* log_formatter;
        std::ostringstream output;
        
    public:
        
        TableLogFormatter( bool is_table , std::string format );
        ~TableLogFormatter();
        
        // Push a new log into the table to be print
        void add( Log* log );
        
        // Get the table to be print
        std::string str();
        
    };
    
}

#endif