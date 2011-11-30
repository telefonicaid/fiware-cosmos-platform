
#include <string>
#include <sstream>
#include <vector>
#include <stdio.h>
#include <cstdarg>

#include "au/time.h"            // au::str_time

#include "Table.h"     // Own definitions

NAMESPACE_BEGIN(au)


double toDouble( std::string value )
{
    return atof(value.c_str());
}

std::string toString( double value )
{
    return au::str("%f",value);
}

std::string str_length( std::string value , int length )
{
    return au::str( au::str( "%%%ds" , length ).c_str() , value.c_str() );
}


#pragma mark TableRow


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

DataSetRow::~DataSetRow()
{
    fields.clearMap();
}

void DataSetRow::set( std::string concept , std::string value )
{
    fields.removeInMap( concept );
    fields.insertInMap( concept , new std::string(value) );
}

int DataSetRow::getLength( std::string field )
{
    std::string value = get( field );
    LM_M(("Getting length of %s = %s --> %lu" , field.c_str() , value.c_str() , value.length() ));
    return value.length();
}        

void DataSetRow::getAllFields( std::set<std::string> &res_fields )
{
    std::set<std::string> _fields;
    
    au::map< std::string , std::string >::iterator it_field;
    for( it_field = fields.begin() ; it_field != fields.end() ; it_field++ )
        res_fields.insert( it_field->first );
}

std::string DataSetRow::get( std::string field )
{
    
    // ^operation for dividing values
    //------------------------------------------------------------------------
    {
        std::vector<std::string> field_components;
        split( field , '^' , field_components );
        
        if( field_components.size() == 2 )
        {
            double num = toDouble( get(field_components[0] ) );
            double den = toDouble( get(field_components[1] ) );
            if( den > 0 )
                return toString( num / den );
            else
                return "0";
        }
        
        if( field_components.size() > 1 )
            return "Error with ^ operation";
    }
    //------------------------------------------------------------------------
    
    // +operation for dividing values
    //------------------------------------------------------------------------
    {
        std::vector<std::string> field_components;
        split( field , '+' , field_components );
        
        if( field_components.size() > 1 )
        {
            double total = 0;
            
            for ( size_t i = 0 ; i < field_components.size() ; i++)
                total+= toDouble( get(field_components[i] ) );
            
            return toString( total );
        }
        
    }
    
    //------------------------------------------------------------------------        
    
    std::string *value = fields.findInMap( field );
    if( !value )
        return "";
    else
        return *value;
}


#pragma mark

void DataSet::add( DataSetRow* row)
{
    rows.push_back(row);
}

DataSet::~DataSet()
{
    for ( size_t i = 0 ; i < rows.size() ; i++ )
        delete rows[i];
    rows.clear();
}

void DataSet::getAllFields( std::vector<std::string> &fields )
{
    std::set<std::string> _fields;
    for ( size_t r = 0 ; r < rows.size() ; r++ )
    {
        DataSetRow *row = rows[r];
        row->getAllFields(_fields);
    }
    
    // Insert in the answer vector
    std::set<std::string>::iterator i;
    for ( i = _fields.begin() ; i != _fields.end() ; i++)
        fields.push_back(*i);
}

size_t DataSet::getNumRecords()
{
    return rows.size();
}

std::string DataSet::getValue( int record , std::string field)
{
    if ( ( record < 0 ) || ( record >= (int)rows.size() ))
        return "";
    return rows[record]->get(field);
}

size_t DataSet::getMaxLength( std::string field , bool title_considered )
{
    size_t max_length = 0;
    
    if( title_considered) 
        max_length = field.length(); // We start with the length of the title
    
    for ( size_t i = 0 ; i < rows.size() ; i++ )
    {
        size_t length = rows[i]->getLength( field );
        if( length > max_length )
            max_length = length;
    }
    
    LM_M(("Getting length of field %s" , field.c_str() , max_length ));
    
    return max_length;
}


void DataSet::set( std::string field , std::string value )
{
    for ( size_t r = 0 ; r < rows.size() ; r++ )
        rows[r]->set( field , value );
}

std::string DataSet::str()
{
    // Get all fields
    std::vector<std::string> fields;
    getAllFields( fields );
    
    // Length of each field
    size_t *length = new size_t[ fields.size() ];
    for( size_t i = 0 ; i < fields.size() ; i++)
        length[i] = getMaxLength( fields[i] , true );
    int num_cols = (int) fields.size();
    
    
    std::ostringstream output;
    
    // Separation line
    output << line( length , num_cols );
    
    // TITLES
    output << "| ";
    for ( size_t c = 0 ; c < fields.size() ; c++)
        // Get the formatted output
        output << str_length( fields[c] , length[c] ) << " | ";
    output << "\n";
    
    // Separation line
    output << line( length , num_cols );
    
    for ( size_t r = 0 ; r < rows.size() ; r++ )
    {
        output << "| ";
        for ( size_t c = 0 ; c < fields.size() ; c++)
        {
            // Get the value containied in the data set
            std::string value = getValue( r, fields[c] );
            
            // Get the formatted output
            output << str_length( value , length[c] ) << " | ";
        }
        output << "\n";
    }
    
    output << line( length , num_cols );
    
    delete[] length;
    
    return output.str();
}


#pragma mark

DataSetFilterColumn::DataSetFilterColumn( std::string field_definition )
{
    left = false;           // By default no left aligment
    format = "string";      // By default string format
    
    std::vector<std::string> components;
    split( field_definition , ',' , components );
    
    if( field_definition.size() == 0 )
    {
        field = "error";
        return;
    }
    
    field = components[0];
    title = components[0]; // Default title
    
    for ( size_t i = 1 ; i < components.size() ; i++ )
        processModifier( components[i] );
    
}

void DataSetFilterColumn::processModifier( std::string modifier )
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
    
    // equal commands
    
    if(  ( (modifier_components[0] == "title" ) || (modifier_components[0] == "t" ) ) )
    {
        title = modifier_components[1];
        return;
    }
    if(  ( (modifier_components[0] == "format" ) || (modifier_components[0] == "f" ) ) )
    {
        format = modifier_components[1];
        return;
    }
    
}

std::string DataSetFilterColumn::getFormatedValue( std::string value )
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
    
    return "Unknown format: " + format;
}

std::string DataSetFilterColumn::getAlignedValue( std::string value , size_t length )
{
    if( left )
    {
        return au::str( au::str("%%-%ds",length).c_str() , value.c_str() );
    }
    else
        return au::str( au::str("%%%ds",length).c_str() , value.c_str() );
}


std::string DataSetFilterColumn::getValue( std::string value , size_t length )
{
    return getAlignedValue( getFormatedValue( value ) , length );
}

std::string DataSetFilterColumn::getTitle( size_t length  )
{
    return getAlignedValue( title , length ); 
}


#pragma mark

DataSetFilter::DataSetFilter(  )
{
}

DataSetFilter::DataSetFilter( std::string definition)
{
    au::CommandLine cmdLine;
    cmdLine.parse( definition );
    
    for ( int i = 0 ; i < cmdLine.get_num_arguments() ; i++)
    {
        std::string field_definition = cmdLine.get_argument(i);
        add( new au::DataSetFilterColumn( field_definition ) );
    }
    
}

DataSetFilter::~DataSetFilter()
{
    for ( size_t c = 0 ;  c < columns.size() ; c++ )
        delete columns[c];
    columns.clear();
}

void DataSetFilter::addAllFields( DataSet * dataSet )
{
    std::vector<std::string> fields;
    dataSet->getAllFields( fields );
    
    // Insert a colum per field
    for( size_t i = 0 ; i < fields.size() ; i++ )
        columns.push_back ( new DataSetFilterColumn( fields[i] ) );
    
}

void DataSetFilter::add( DataSetFilterColumn * column)
{
    columns.push_back(column);
}


DataSet* DataSetFilter::transform( DataSet* input )
{
    // Output data set
    DataSet* output = new DataSet();
    
    // Length of each field
    int *length = new int[ columns.size() ];
    for( size_t i = 0 ; i < columns.size() ; i++)
        length[i] = input->getMaxLength( columns[i]->field , false );
    
    
    // For each record at the input, create an output 
    for( size_t r = 0 ; r < input->rows.size() ; r++)
    {
        DataSetRow* inputRow = input->rows[r];
        DataSetRow* outputRow = new DataSetRow();
        
        for( size_t c = 0 ; c < columns.size() ; c++ )
        {
            
            // Get the value containied in the data set
            std::string value = inputRow->get( columns[c]->field );
            
            // Get the formatted output
            outputRow->set( columns[c]->title , columns[c]->getValue( value , length[c] ) );
            
        }
        
        output->add( outputRow );
    }
    
    
    delete[] length;
    return output;
    
    
}

std::string DataSetFilter::str( DataSet* input )
{
    return str( input , 0 );
}

std::string DataSetFilter::str( DataSet* input , int limit  )
{
    int num = 0;
    std::ostringstream output;
    
    // Length of each field
    size_t *length = new size_t[ columns.size() ];
    for( size_t c = 0 ; c < columns.size() ; c++)
    {
        length[c] = 0;
        
        for( size_t r = 0 ; r < input->rows.size() ; r++)
        {
            DataSetRow* inputRow = input->rows[r];
            
            std::string value = inputRow->get( columns[c]->field );
            std::string formated_value = columns[c]->getValue( value , length[c] );
            
            size_t _length = formated_value.length();
            if(_length > length[c] )
                length[c] = _length;
        }
        
        {
            size_t _length = columns[c]->title.length();
            if(_length > length[c] )
                length[c] = _length;
        }
        
    }
    
    // Separation line
    output << line( length , columns.size() );
    
    // TITLES
    output << "| ";
    for ( size_t c = 0 ; c < columns.size() ; c++)
        // Get the formatted output
        output << str_length( columns[c]->title , length[c] ) << " | ";
    output << "\n";
    
    // Separation line
    output << line( length , columns.size() );
    
    // For each record at the input, create an output 
    for( size_t r = 0 ; r < input->rows.size() ; r++)
    {
        DataSetRow* inputRow = input->rows[r];
        
        output << "| ";
        for( size_t c = 0 ; c < columns.size() ; c++ )
        {
            // Get the value containied in the data set
            std::string value = inputRow->get( columns[c]->field );
            std::string formated_value = columns[c]->getValue( value , length[c] );
            
            // Get the formatted output
            output << formated_value << " | ";
        }
        output << "\n";
        num++;
        
        if( limit > 0 )
            if( num >= limit ) 
                break;
    }
    
    // Separation line
    output << line( length , columns.size() );
    
    delete[] length;
    return output.str();        
    
}

NAMESPACE_END