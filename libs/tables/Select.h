
#ifndef _H_AU_TABLE_SELECT
#define _H_AU_TABLE_SELECT

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

class TableRow;
class Table;
class TreeItem;

bool compare_strings( std::string& a , std::string& b);


class SelectCondition
{
    friend class Table;
    friend class TableRow;
    
public:
    
    std::string name;
    std::string value;
    
    SelectCondition( std::string _name , std::string _value )
    {
        name = _name;
        value = _value;
    }
    
    bool check( TableRow *row );
    bool check( TreeItem *tree );
    
};


// Class to hold how to format a particular column

class SelectTableColumn
{
    std::string name;      // Name of the column to display
    std::string title;     // Title to display

    bool left;             // Left aligned content
    
    size_t columnd_id;     // Columns identifier

    friend class Table;
    
    typedef enum
    {
        sum,
        max,
        min,
        min_max,
        vector,
        different
    } SelectTableColumnGroup;

    typedef enum
    {
        format_string,
        format_uint64,
        format_time,
        format_timestamp,
        format_double,
        format_percentadge,
    } SelectTableColumnFormat;
    
    
    SelectTableColumnGroup group;
    SelectTableColumnFormat format;
    
public:
    
    SelectTableColumn( std::string field_definition );
    
    std::string getName();
    std::string getTitle();
    bool getLeft();
    
    std::string transform( StringVector& values );
    
    std::string transform_string( StringVector& values );
    std::string transform_time( StringVector& values );
    std::string transform_uint64( StringVector& values );
    std::string transform_double( StringVector& values );
    std::string transform_percentadge( StringVector& values );
    
private:
    
    void processModifier( std::string modifier );
    
    std::string simple_transform( std::string value );
  
    
    void sort( StringVector& values );
    std::string str_sum( StringVector& values );

};


class SelectTableInformation
{
    
public:
    
    std::string title;                           // Title of the column ( when printing )
    std::vector<SelectTableColumn> columns;      // Columns to select or print
    std::vector<SelectCondition> conditions;     // Conditions to meet by each row

    size_t limit;                                // Max number of records ( in the final print )
    
    StringVector group_columns;                  // Group records by this ( if any )
    StringVector divide_columns;                 // If not grouped, it can be divided
    StringVector sort_columns;                   // If not grouped nor divided, it is sorted by these
        
    SelectTableInformation()
    {
        limit = 0;        
    }
    
    SelectTableInformation( Table* table );
    
    
    void addColumn( std::string definition )
    {
        columns.push_back( SelectTableColumn( definition ) );
    }
    
    void addColumns( au::StringVector _columns )
    {
        for ( size_t i = 0 ; i < _columns.size() ; i++ )
            addColumn(_columns[i]);
    }
    
    // Get a vector with all columns names
    StringVector getColumns()
    {
        StringVector tmp;
        for ( size_t i = 0 ; i < columns.size() ; i++ )
            tmp.push_back( columns[i].getName() );
        return tmp;
    }
    
    // Sorting
    bool isSortRequired()
    {
        return sort_columns.size()>0;
    }
    
    // Grouping
    bool isGroupRequired()
    {
        return group_columns.size() > 0;
    }
    
    // Division 
    bool isDivideRequired();
    bool checkChangeDivision( TableRow* row , TableRow* row2);

    // Conditions
    bool check( TableRow *row )
    {
        for ( size_t i = 0 ; i < conditions.size() ; i++ )
            if( !conditions[i].check(row) )
                return false;
        return true;
    }
    
    void add_conditions( std::string conditions );
    
};


class SelectTreeInformation
{
    
public:
    
    std::string name;                             // Name of the node we are looking for
    std::vector<SelectCondition> conditions;      // Conditions to meet by each item
    bool recursevely;                             // Search recursevely
    bool valid;
    
    SelectTreeInformation( std::string description )
    {
        size_t pos_name_begin = 1;
        if( description.substr(0,2) == "//" )
        {
            pos_name_begin = 2;
            recursevely = true;
        }
        else
        {
            recursevely = false;
        }
        
        // Search for [property]
        size_t pos_1 = description.find('[', 0 );
        size_t pos_2 = description.find(']', 0 );
        
        if( pos_1 == std::string::npos )
        {
            // No conditions
            name = description.substr( pos_name_begin );
        }
        else if( ( pos_2 != std::string::npos ) && ( pos_1 <= pos_2 ) )
        {
            // Main mode name
            name = description.substr( pos_name_begin , pos_1-pos_name_begin );

            // Filter for all the nodes
            std::string conditions_description = description.substr( pos_1+1 , pos_2 - pos_1 - 1 );
            addConditions( conditions_description );
            
        }
        
    }
    
    void addConditions( std::string conditions_description )
    {
        StringVector conditions_descriptions = StringVector::parseFromString(conditions_description, ',');
        for ( size_t i = 0 ; i < conditions_descriptions.size() ; i++ )
        {
            StringVector parts = StringVector::parseFromString(conditions_descriptions[i], '=');
            if ( parts.size() == 2 )
                addCondition(parts[0], parts[1] );
        }
    }
    
    void addCondition( std::string field , std::string value )
    {
        conditions.push_back(SelectCondition(field,value) );
    }

    
    bool check( TreeItem* tree )
    {
        
        for ( size_t i = 0 ; i < conditions.size() ; i++ )
        {
            LM_M(("Cheking condition %s=%s" , conditions[i].name.c_str() , conditions[i].value.c_str() ));
            if( !conditions[i].check(tree) )
                return false;
        }
        return true;
        
    }
    
    std::string str()
    {
        return au::str("SELECT TREE %s (%c)" , name.c_str() , recursevely?'R':' ' );
    }
    
};

NAMESPACE_END
NAMESPACE_END

#endif