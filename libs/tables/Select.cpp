
#include "Table.h"
#include "Tree.h"

#include "Select.h" // Own interface

NAMESPACE_BEGIN(au)
NAMESPACE_BEGIN(tables)

bool compare_strings( std::string& a , std::string& b)
{
    if( a == "*" )
        return true;
    if( b == "*" )
        return true;

    if( a[a.length()-1] == '*' )
    {
        // Starts with
        if( b.length() < (a.length()-1) )
            return false;
        for( size_t i = 0 ; i < (a.length()-1) ; i++ )
            if( b[i] != a[i] )
                return false;
        return true;
    }
    
    if( a[0] == '*' )
    {
        // Ends with
        if( b.length() < (a.length()-1) )
            return false;
        for( size_t i = 0 ; i < (a.length()-1) ; i++ )
            if( b[ b.length() - (a.length()-1) + i  ] != a[i] )
                return false;
        return true;
    }    
    
    if ( a==b)
        return true;
    
    return false;
}


#pragma mark SelectRowCondition

bool SelectCondition::check( TableRow *row )
{
    std::string row_value = row->get(name);
    return compare_strings( value , row_value );
}

bool SelectCondition::check( TreeItem *tree )
{
    return (tree->getFirstNodeValue(name) == value);
}


SelectTableColumn::SelectTableColumn( std::string field_definition )
{
    left = false;            // By default no left aligment
    format = format_string;  // By default string format
    
    group = sum;             // Default criteria
    
    std::vector<std::string> components;
    split( field_definition , ',' , components );
    
    if( field_definition.size() == 0 )
    {
        title = "error";
        return;
    }
    
    title = components[0];
    name = components[0];
    
    // Process other modifiers
    for ( size_t i = 1 ; i < components.size() ; i++ )
        processModifier( components[i] );
}

void SelectTableColumn::processModifier( std::string modifier )
{
    
    // Single commands
    if( ( modifier == "l" ) || ( modifier == "left") )
    {
        left = true;
        return;
    }
    
    if( ( modifier == "sum" ) || ( modifier == "s") )
    {
        group=sum;
        return;
    }

    if( ( modifier == "different" ) || ( modifier == "d") )
    {
        group=different;
        return;
    }
    
    if( ( modifier == "vector" ) || ( modifier == "v") )
    {
        group=vector;
        return;
    }
    
    if( ( modifier == "max" ) )
    {
        group=max;
        return;
    }
    if( ( modifier == "min" ) )
    {
        group=min;
        return;
    }
    
    std::vector<std::string> modifier_components;
    split( modifier , '=' , modifier_components );
    
    if ( modifier_components.size() != 2 )
        return;
    
    if(  ( (modifier_components[0] == "format" ) || (modifier_components[0] == "f" ) ) )
    {
        std::string str_format = modifier_components[1];
        
        if( str_format == "string" )
            format = format_string;
        if( str_format == "uint64" )
            format = format_uint64;
        if( str_format == "uint" )
            format = format_uint64;
        if( str_format == "double" )
            format = format_double;
        if( str_format == "per" )
            format = format_percentadge;
        if( str_format == "time" )
            format = format_time;
        return;
    }
    
    if(  ( (modifier_components[0] == "title" ) || (modifier_components[0] == "t" ) ) )
    {
        title = modifier_components[1];
        return;
    }
    
}

std::string SelectTableColumn::getName()
{
    return  name;
}

std::string SelectTableColumn::getTitle()
{
    return title;
}

bool SelectTableColumn::getLeft()
{
    return left;
}

std::string SelectTableColumn::simple_transform( std::string value )
{
    switch (format) 
    {
        case format_string:       return value;
        case format_uint64:       return au::str( strtoll( value.c_str() , (char **)NULL, 10) );
        case format_double:       return au::str( "%0.2f", strtof( value.c_str() , (char **)NULL ));
        case format_time:         return au::time_string( strtoll( value.c_str() , (char **)NULL, 10) );
        case format_percentadge:  return au::percentage_string( atof( value.c_str() ) );
    }
    
    return "<ERROR>";
}

std::string SelectTableColumn::transform( StringVector& values )
{
    
    if( values.size() == 0 )
        return "<no values>";
    
    if( values.size() == 1 )
        return simple_transform( values[0] );
    
    switch (group) 
    {
        case vector:
        {
            std::ostringstream output;
            output << "[ ";
            for ( size_t i = 0 ; i < values.size() ; i++ )
                output << simple_transform( values[i] ) << " ";
            output << "]";
            return output.str();
        }

        case different:
        {
            values.unique();
            if( values.size() == 1 )
                return values[0];
            
            std::ostringstream output;
            output << "[ ";
            for ( size_t i = 0 ; i < values.size() ; i++ )
                output << simple_transform( values[i] ) << " ";
            output << "]";
            return output.str();
        }
        
        case min:
        {
            sort(values);
            return simple_transform( values[values.size()-1] );
        }
            
        case max:
        {
            sort(values);
            return simple_transform( values[0] );
        }
    
        case min_max:
        {
            std::string min = simple_transform( values[values.size()-1] );
            std::string max = simple_transform( values[0] );
            return au::str("[ %s - %s ]" , min.c_str() , max.c_str());
        }
    
        case sum:
        {
            return str_sum(values);
        }
    }    
    
    return "<Error>";
    
}

void SelectTableColumn::sort( StringVector& values )
{
    if ( ( format == format_uint64 ) || ( format == format_time ) )
    {
        std::vector<size_t> _values;
        for( size_t i = 0 ; i < values.size() ; i++ )
            _values.push_back(strtoll( values[i].c_str() , (char **)NULL, 10));
        std::sort(_values.begin(), _values.end());
        
        values.clear();
        for ( size_t i = 0 ; i < _values.size() ; i++ )
            values.push_back( au::str("%lu",_values[i] ) );
    }
    
    if ( ( format == format_double ) || ( format == format_percentadge ) )
    {
        std::vector<double> _values;
        for( size_t i = 0 ; i < values.size() ; i++ )
            _values.push_back(strtof( values[i].c_str() , (char **)NULL));
        std::sort(_values.begin(), _values.end());
        
        values.clear();
        for ( size_t i = 0 ; i < _values.size() ; i++ )
            values.push_back( au::str("%f",_values[i] ) );
    }
}

std::string SelectTableColumn::str_sum( StringVector& values )
{
    if ( ( format == format_uint64 ) || ( format == format_time ) )
    {
        size_t total = 0;
        for( size_t i = 0 ; i < values.size() ; i++ )
            total += strtoll( values[i].c_str() , (char **)NULL, 10);
        return simple_transform( au::str("%lu",total) );
    }
    
    if ( ( format == format_double ) || ( format == format_percentadge ) )
    {
        size_t total = 0;
        for( size_t i = 0 ; i < values.size() ; i++ )
            total += strtof( values[i].c_str() , (char **)NULL);
        return simple_transform( au::str("%f",total) );
    }
    

    for ( size_t i = 1 ; i < values.size() ; i++ )
        if ( values[i] != values[i-1] )
            return "?";
    return values[0];
}



#pragma mark SelectTableInformation

SelectTableInformation::SelectTableInformation( Table * table )
{
    // All fields
    for ( size_t i = 0 ; i < table->getNumColumns() ; i++ )
        columns.push_back( SelectTableColumn( table->getColumn(i) ) );
    limit = 0;        

}

bool SelectTableInformation::isDivideRequired()
{
    return divide_columns.size() > 0;
}

bool SelectTableInformation::checkChangeDivision( TableRow* row , TableRow* row2)
{
    return row->compare( row2 , divide_columns ) != 0;
}



NAMESPACE_END
NAMESPACE_END
