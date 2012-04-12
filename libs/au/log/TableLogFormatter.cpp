
#include "au/log/Log.h"
#include "au/log/LogFormatter.h"
#include "TableLogFormatter.h" // Own interface

namespace au 
{
    TableLogFormatter::TableLogFormatter( bool is_table , bool _is_reverse, std::string format )
    {
        // Keep this information
        is_reverse = _is_reverse;
        
        if( is_table )
        {
            LM_V(("TableLogFormatter table"));
            
            // Table fields
            table_fields = split_using_multiple_separators( format , "|:;,-/\[] ()." );
            
            std::string table_definition;
            for ( size_t i = 0 ; i < table_fields.size() ; i++ )
            {
                if( table_fields[i].length() > 0 )
                    table_definition.append(  au::str("%s,left|", table_fields[i].c_str() ) );
            }
            
            table = new au::tables::Table( table_definition );
            log_formatter = NULL;
            
            LM_V(("TableLogFormatter table OK"));
            
        }
        else
        {
            LM_V(("TableLogFormatter no table"));

            log_formatter = new LogFormatter( format );
            table = NULL;
            
            LM_V(("TableLogFormatter no table OK"));

        }
        
        
    }
    
    TableLogFormatter::~TableLogFormatter()
    {
        if( table )
            delete table;
        if( log_formatter )
            delete log_formatter;
    }
    
    void TableLogFormatter::add( Log* log )
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
    
    std::string TableLogFormatter::str()
    {
        
 
        
        if( table )
        {
            if( is_reverse )
                table->reverseRows();
            
            return table->str();
        }
        else
        {
            
            std::string lines = output.str();
            if( is_reverse )
                return au::reverse_lines( lines  );
            else
                return lines;
        }
        
    }

}