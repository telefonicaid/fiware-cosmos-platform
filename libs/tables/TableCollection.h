
#ifndef _H_AU_TABLE_COLLECTION
#define _H_AU_TABLE_COLLECTION

#include <stdio.h>             /* sprintf */
#include <string>				// std::string
#include <sys/time.h>           // struct timeval
#include <sstream>              // std::ostringstream
#include "logMsg/logMsg.h"             // LM_W

#include "au/CommandLine.h"
#include "au/ErrorManager.h"        // au::ErrorManager
#include "au/map.h"                 // au::map
#include "au/string.h"              // au::str(...)
#include "au/ConsoleAutoComplete.h"

#include "au/namespace.h"


NAMESPACE_BEGIN(au)

NAMESPACE_BEGIN(table)

class StringVector : public std::vector<std::string>
{
public:
    
    StringVector()
    {
    }
    
    StringVector( std::string value )
    {
        push_back(value);
    }
    StringVector( std::string value , std::string value2 )
    {
        push_back(value);
        push_back(value2);
    }
    StringVector( std::string value , std::string value2 , std::string value3 )
    {
        push_back(value);
        push_back(value2);
        push_back(value3);
    }
    StringVector( std::string value , std::string value2 , std::string value3 , std::string value4 )
    {
        push_back(value);
        push_back(value2);
        push_back(value3);
        push_back(value4);
    }
    
    static StringVector parseFromString( std::string line )
    {
        StringVector string_vector;
        au::split(line, ' ', string_vector );
        return string_vector;
    }

    static StringVector parseFromString( std::string line , char separator )
    {
        StringVector string_vector;
        au::split(line, separator , string_vector );
        return string_vector;
    }
    
};


class TableRow
{
    StringVector values;
    
public:
    
    TableRow( StringVector _values )
    {
        for ( size_t i = 0 ; i < _values.size() ; i++ )
            values.push_back(_values[i]);
    }
    
    TableRow( TableRow* table_row )
    {
        for ( size_t i = 0 ; i < table_row->values.size() ; i++ )
            values.push_back( table_row->values[i] );
    }
    
    void set( int column , std::string value )
    {
        values[column] = value;
    }
    
    std::string get( int column )
    {
        if ( ( column < 0 ) || ( column >= (int)values.size() ))
            return "";
        else
            return values[column];
    }
    
    
    // Insert values in rows...
    void push( std::string value )
    {
        values.push_back(value);
    }
    
};

// Class to hold a simple table of values

class TableColumn
{
    std::string title;     // Title to display
    std::string format;    // Format of the content
    bool left;             // Left aligned content
    
public:
    
    TableColumn( std::string field_definition )
    {
        left = false;           // By default no left aligment
        format = "string";      // By default string format
        
        std::vector<std::string> components;
        split( field_definition , ',' , components );
        
        if( field_definition.size() == 0 )
        {
            title = "error";
            return;
        }
        
        title = components[0];
        
        // Process other modifiers
        for ( size_t i = 1 ; i < components.size() ; i++ )
            processModifier( components[i] );
    }
    
    TableColumn( TableColumn *table_column)
    {
        title = table_column->title;
        format = table_column->format;
        left = table_column->left;
    }
    
    void processModifier( std::string modifier )
    {
        
        // Single commands
        if( ( modifier == "l" ) || ( modifier == "left") )
        {
            left = true;
            return;
        }
        
        std::vector<std::string> modifier_components;
        split( modifier , '=' , modifier_components );
        
        if ( modifier_components.size() != 2 )
            return;

        if(  ( (modifier_components[0] == "format" ) || (modifier_components[0] == "f" ) ) )
        {
            format = modifier_components[1];
            return;
        }

        if(  ( (modifier_components[0] == "title" ) || (modifier_components[0] == "t" ) ) )
        {
            title = modifier_components[1];
            return;
        }
        
    }
    
    
    std::string getTitle()
    {
        return title;
    }
    
    std::string getFormat()
    {
        return format;
    }
    
    bool getLeft()
    {
        return left;
    }
    
    std::string transform( std::string value )
    {
        
        if (format == "string" )
            return value;
        else if ( format == "uint64" )
        {
            size_t _value = strtoll( value.c_str() , (char **)NULL, 10);
            return au::str( _value );
        }
        else if ( format == "time" )
        {
            size_t _value = strtoll( value.c_str() , (char **)NULL, 10);
            return au::time_string( _value );
        }
        else if ( format == "double2" )
        {
            double _value = strtof( value.c_str() , (char **)NULL );
            return au::str( "%0.2f", _value );
        }
        else if ( ( format == "percentadge" ) || ( format == "per" ) )
        {
            double p = atof( value.c_str() );
            return au::percentage_string(p);
        }
        
        return value;
    }
    
};

class Table
{
    std::vector<TableColumn*> columns;   // Columns of this table with format definition
    std::vector<TableRow* > rows;         // List of rows
    
public:
    
    Table( StringVector titles )
    {
        for ( size_t i = 0 ; i < titles.size() ; i++ )
            columns.push_back( new TableColumn( titles[i] ) );
    }
    
    Table( std::vector<TableColumn*>& _columns )
    {
        for ( size_t i = 0 ; i < _columns.size() ; i++ )
            columns.push_back( _columns[i] );
    }
    
    Table( Table* table )
    {
        for( size_t c = 0 ; c < table->columns.size() ; c++)
            columns.push_back( new TableColumn( table->columns[c] ) );
        
        for( size_t r = 0 ; r < table->rows.size() ; r++ )
            rows.push_back( new TableRow( table->rows[r] ) );
        
    }

    std::string getValue( size_t r , size_t c )
    {
        if( ( r == (size_t)-1) || ( c == (size_t)-1) )
            return "";
        
        if( r < 0 )
            return "";
        if ( r > rows.size() )
            return "";
        
        return rows[r]->get(c);
    }
    
    std::string getValue( int r , std::string column_name )
    {
        return getValue(r, getColumn(column_name) );
    }
    
    size_t getColumn( std::string title )
    {
        for( size_t i = 0 ; i < columns.size() ; i++ )
            if( columns[i]->getTitle() == title )
                return i;
        
        return (size_t)-1;
    }
    
    TableColumn* duplicateColumn( std::string title )
    {
        size_t c = getColumn(title);
        if( c == (size_t)-1)
            return NULL;
        
        return new TableColumn( columns[c] );
    }
    
    
    void addRow( StringVector string_vector )
    {
        addRow( new TableRow( string_vector ) );
    }
    
    void addRow( TableRow * row )
    {
        rows.push_back(row);
    }
    
    Table *duplicate()
    {
        return new Table( this );
    }
    
    std::string str()
    {
        if( columns.size() == 0 )
            return au::string_in_color( "ERROR: No columns in this table" , "red" );
        
        size_t *column_width = (size_t*) malloc( sizeof(size_t) * columns.size() );
        
        for( size_t c = 0 ; c < columns.size() ; c++)
        {
            // Init column witdh
            column_width[c] = 0;
            
            // Search all the values to determine witdh
            for( size_t r = 0 ; r < rows.size() ; r++ )
            {
                TableRow* row = rows[r];
                
                std::string value = columns[c]->transform( row->get(c) );
                
                size_t _length = value.length();
                if(_length > column_width[c] )
                    column_width[c] = _length;
            }
            
            // Title also computes to determine max length
            {
                size_t _length = columns[c]->getTitle().length();
                if(_length > column_width[c] )
                    column_width[c] = _length;
            }
            
        }
        
        // Output string
        std::ostringstream output;
        
        // Separation line
        output << line( column_width , columns.size() );
        
        // TITLES
        output << "| ";
        for ( size_t c = 0 ; c < columns.size() ; c++)
            output << string_length( columns[c]->getTitle() , column_width[c] , columns[c]->getLeft() ) << " | ";
        output << "\n";
        
        // Separation line
        output << line( column_width , columns.size() );
        
        // For each record at the input, create an output 
        for( size_t r = 0 ; r < rows.size() ; r++)
        {
            TableRow* row = rows[r];
            
            output << "| ";
            for( size_t c = 0 ; c < columns.size() ; c++ )
            {
                // Get the value containied in the data set
                std::string value = columns[c]->transform( row->get(c) );
                
                // Get the formatted output
                output << string_length(value , column_width[c] , columns[c]->getLeft() ) << " | ";
            }
            output << "\n";
        }
        
        // Separation line
        output << line( column_width , columns.size() );
        
        
        free(column_width);
        return output.str();
    }
    
    size_t getNumRows()
    {
        return rows.size();
    }
    
    size_t getNumColumns()
    {
        return columns.size();
    }
    
    Table* getColumnDescriptionTable()
    {
        // Example to play with
        Table *tmp = new Table( StringVector("Record,left" , "Format" ) );
        
        for( size_t c = 0 ; c < columns.size() ; c++ )
        {
            std::string title = columns[c]->getTitle();

            std::string format = columns[c]->getFormat();
            bool _left = columns[c]->getLeft();

            if ( _left )
                tmp->addRow( StringVector( title , au::str("(left) %s" , format.c_str() ) ) );
            else
                tmp->addRow( StringVector( title , format ) );
            
        }

        return tmp;
    }
    
private:
    
    std::string line( size_t *length , int num_cols )
    {
        std::ostringstream output;
        
        output << "+-";
        // Separation line
        for ( int c = 0 ; c < num_cols ; c++)
        {
            for( size_t i = 0 ; i < length[c] ; i++ )
                output << "-";
            
            // Get the formatted output
            if( c != ( num_cols-1) )
                output << "-+-";
        }
        output << "-+";
        output << "\n";
        
        return output.str();
    }
    
    std::string string_length( std::string value , int width , bool left )
    {
        if( left )
            return au::str( au::str("%%-%ds",width).c_str() , value.c_str() );
        else
            return au::str( au::str("%%%ds",width).c_str() , value.c_str() );
    }
    
    
};


// Ecosystem of tables to work with them

class TableCollection
{
    au::map<std::string, Table> tables;
    
public:
    
    TableCollection()
    {
        // Example to play with
        Table *tmp = new Table( StringVector("name" , "telephone" , "city") );
        tmp->addRow( StringVector("Andreu","7720167","Barcelona") );
        tmp->addRow( StringVector("Pepe","7720167","Madrid") );
        tmp->addRow( StringVector("Juan","847329847","Merida") );
        tmp->addRow( StringVector("Maria","847329847","Merida") );
        addTable( "prova" , tmp );
        
    }
    
    // Add a table to the collection
    void addTable( std::string name , Table* table )
    {
        tables.removeInMap(name);
        tables.insertInMap(name, table);
    }
    
    Table* getTableOfTables()
    {
        Table *tmp = new Table( StringVector("Name,left" , "#Columns,format=uint64" , "#Rows,format=uint64" ) );
        
        au::map<std::string, Table>::iterator it_tables;
        for (it_tables=tables.begin();it_tables!=tables.end();it_tables++)
        {
            std::string name = it_tables->first;
            Table *table = it_tables->second;
            tmp->addRow( 
                        StringVector( name 
                                     , au::str("%lu" , table->getNumColumns() ) 
                                     , au::str("%lu" , table->getNumRows() )   
                                     )
                        );
        }
        
        return tmp;
    }
    
    
    Table* getErrorTable( int error_code , std::string error_message )
    {
        Table *tmp = new Table( StringVector( "error" , "description" ) );
        tmp->addRow(  StringVector( au::str("%d",error_code) , au::string_in_color(error_message, "red")  ) );
        return tmp;
    }

    Table* getTableFromSelect( std::string command )
    {
        
        // select field field2 field3 from table where worker=56 name=andreu
        StringVector fields;
        std::string from_table;
        StringVector conditions;
        
        // ----------------------------------------------------------------
        
        size_t pos = 1;
        StringVector words = StringVector::parseFromString( command );
        
        while( ( pos < words.size() ) && ( words[pos] != "from" ) )
            fields.push_back( words[pos++] );
        
        if ( pos >= ( words.size() - 1 ) )
            return getErrorTable( 1 , "select statement without from clause");
        
        pos++; // Skip the word "from"
        
        from_table = words[pos++];
        
        if( pos < words.size() )
        {
            if( words[pos++] != "where" )
                return getErrorTable( 1 , "Only clause where is suported after table");
            
            while( pos < words.size()   )
                conditions.push_back( words[pos++] );
        }
        
        // ----------------------------------------------------------------
        
        Table *fromTable = tables.findInMap( from_table );
        if( !fromTable )
            return getErrorTable( 1 , au::str("Unknown table %s" , from_table.c_str() ) );
        
        StringVector main_fields;
        
        // Add columns
        std::vector<TableColumn*> columns;
        for ( size_t c = 0 ; c < fields.size() ; c++ )
        {
            StringVector components = StringVector::parseFromString( fields[c] , ',' );
            std::string field_name = components[0];
            
            // Record the main fields to reconstruct records
            main_fields.push_back( field_name );
            
            TableColumn * table_column = fromTable->duplicateColumn( field_name );
            if( !table_column )
                table_column = new TableColumn( field_name );
            
            for (size_t i = 1 ; i < components.size() ; i++ )
                table_column->processModifier( components[i] );
            
            
            columns.push_back( table_column );
        }
        
        // Create the result table
        Table* result = new Table( columns );
        
        // Look for records
        for (size_t r = 0 ; r < fromTable->getNumRows() ; r++ )
        {
            // Collect values for this record
            StringVector values;
            for ( size_t f = 0 ; f < main_fields.size() ; f++ )
                values.push_back( fromTable->getValue( r , main_fields[f] ) );
            
            // create a new record for the results table
            result->addRow( new TableRow(values) );
        }
        
        return result;
        
    }
    
    void autoCompleteTables( au::ConsoleAutoComplete* info )
    {
        au::map<std::string, Table>::iterator it_tables;
        for (it_tables=tables.begin();it_tables!=tables.end();it_tables++)
        {
            std::string name = it_tables->first;
            info->add( name );
        }
    }
    
    
    void autoComplete( au::ConsoleAutoComplete* info )
    {
        if ( info->completingFirstWord() )
        {
            // Add console commands
            info->add("show_tables");
            info->add("info");
            info->add("print_table");
            info->add("save");
            return;
        }
        
        if ( info->completingSecondWord("info") )
            autoCompleteTables(info);    

        if ( info->completingSecondWord("print_table") )
            autoCompleteTables(info);    
        
    }

    
    std::string runCommand( std::string command )
    {
        std::ostringstream output;
        
        au::CommandLine cmdLine;
        cmdLine.parse(command);
        
        if( cmdLine.get_num_arguments() == 0)
            return "No command";
        
        std::string mainCommand = cmdLine.get_argument(0);
        
        if( mainCommand == "show_tables" )
        {
            Table* table = getTableOfTables();
            addTable("result",  table );
            return table->str();
        }

        if( mainCommand == "info" )
        {
            if( cmdLine.get_num_arguments() < 2 )
                return au::string_in_color( 
                                           "Usage: info <table> ( run show_tables to get a list of tables )" 
                                           , "red" 
                                           );
            
            std::string table_name = cmdLine.get_argument(1);
            Table* table = tables.findInMap( table_name );
            if( !table )
                return au::str("Unkown table %s" , table_name.c_str() );

            Table* result = table->getColumnDescriptionTable();
            
            addTable("result",  result );
            return result->str();
        }
        
        if( mainCommand == "print_table" )
        {
            if( cmdLine.get_num_arguments() < 2 )
                return au::string_in_color(
                                           "Usage: show_table <table> ( run show_tables to get a list of tables )"
                                           ,"red"
                                           );
            
            std::string table_name = cmdLine.get_argument(1);
            Table* table = tables.findInMap( table_name );
            if( !table )
                return au::str("Unkown table %s" , table_name.c_str() );

            return table->str();
        }
        
        if ( mainCommand == "save" )
        {
            if( cmdLine.get_num_arguments() < 2 )
                return au::string_in_color( "Usage: save <new_table_name>" , "red" );
            
            std::string table_name = "result";
            Table* table = tables.findInMap( table_name );
            if ( !table )
                return au::string_in_color("No table result to save","red");

            addTable( cmdLine.get_argument(1) ,  table->duplicate() );
            return "Ok";
            
        }
        
        if ( mainCommand == "select" )
        {
            Table* result = getTableFromSelect( command );
            
            addTable("result", result );
            return result->str();
            
        }

        if ( mainCommand == "group" )
        {
            // group field,sum field2,sum field3,average from table by field
        }
        
        
        return "Unknown command";
        
    }
    
    
};

NAMESPACE_END

NAMESPACE_END


#endif