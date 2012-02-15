
#ifndef _H_AU_TABLE_TABLE
#define _H_AU_TABLE_TABLE

#include <stdio.h>             /* sprintf */
#include <string>				// std::string
#include <sys/time.h>           // struct timeval
#include <sstream>              // std::ostringstream
#include "logMsg/logMsg.h"             // LM_W

#include "au/StringVector.h"
#include "au/CommandLine.h"
#include "au/simple_map.h"
#include "au/ErrorManager.h"        // au::ErrorManager
#include "au/map.h"                 // au::map
#include "au/string.h"              // au::str(...)
#include "au/ConsoleAutoComplete.h"

#include "au/namespace.h"


NAMESPACE_BEGIN(au)
NAMESPACE_BEGIN(tables)


class Table;
class SelectTableInformation;
class SelectCondition;


class TableRow
{
    au::simple_map<std::string,std::string> values;
    
    friend class DataBase;
    
public:
    
    TableRow( StringVector _columns , StringVector _values );
    TableRow( TableRow* table_row );

    void set( std::string name , std::string value );
    std::string get( std::string name );
    
    // Function to compare two rows
    int compare( TableRow* row , StringVector &sort_columns );
    
    // get a table with all information
    Table* getTable();
};

class Table
{
    StringVector columns;                 // List of column names
    StringVector formats;                 // List of default format for each column
    std::vector<TableRow* > rows;         // List of rows
    
    friend class DataBase;
    
public:
    
    Table( StringVector _columns , StringVector formats );
    Table( StringVector _columns );
    Table( Table* table );

    ~Table();
    
    void addRow( StringVector string_vector );
    void addRow( TableRow * row );
    
    std::string getValue( size_t r , size_t c );
    std::string getValue( int r , std::string column_name );

    size_t getColumn( std::string title );
    std::string getColumn( size_t pos );
    std::string getFormatForColumn( size_t pos );
    
    std::string str( );
    std::string str( std::string title );
    std::string str( SelectTableInformation *select_table_information );

    std::string strSortedAndGrouped( std::string title , std::string field);    
    
    Table* selectTable( SelectTableInformation *select_table_information );
    
    size_t getNumRows();
    size_t getNumColumns();

    // Description table
    Table* getColumnDescriptionTable();

    // Generic sort function
    void sort( StringVector &sort_columns );

    StringVector getValuesFromColumn( std::string name );
    StringVector getValuesFromColumn( std::string name , SelectCondition* condition );
    
private:
    
    std::string line( std::vector<size_t> &length );
    std::string string_length( std::string value , int width , bool left );
  
    size_t getMaxWidthForColumn( size_t c )
    {
        size_t column_width = 0;
        
        // Search all the values to determine witdh
        for( size_t r = 0 ; r < rows.size() ; r++ )
        {
            std::string value = rows[r]->get( getColumn(c) );
            
            size_t _length = value.length();
            if(_length > column_width )
                column_width = _length;
        }
        return column_width;
    }
    
};


NAMESPACE_END
NAMESPACE_END


#endif