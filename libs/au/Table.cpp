
#include <string>
#include <sstream>
#include <vector>
#include <stdio.h>
#include <cstdarg>

#include "au/time.h"            // au::str_time

#include "Table.h"     // Own definitions

namespace au {

    
    double toDouble( std::string value )
    {
        return atof(value.c_str());
    }
    
    std::string toString( double value )
    {
        return au::str("%f",value);
    }
    

    
#pragma mark TableRow
    
    
    DataSetRow::~DataSetRow()
    {
        fields.clearMap();
    }
    
    void DataSetRow::set( std::string concept , std::string value )
    {
        fields.removeInMap( concept );
        fields.insertInMap( concept , new std::string(value) );
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
    
    int DataSetRow::length( std::string field )
    {
        std::string value = get( field );
        return value.length();
    }        
    
    void DataSetRow::getAllFields( std::set<std::string> &res_fields )
    {
        std::set<std::string> _fields;
        
        au::map< std::string , std::string >::iterator it_field;
        for( it_field = fields.begin() ; it_field != fields.end() ; it_field++ )
            res_fields.insert( it_field->first );
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
    
    
#pragma mark
    
    TableColumn::TableColumn( std::string field_definition )
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

    void TableColumn::processModifier( std::string modifier )
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
    
    std::string TableColumn::getFormatedValue( std::string value )
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

    std::string TableColumn::getAlignedValue( std::string value )
    {
        if( left )
        {
            return au::str( au::str("%%-%ds",length).c_str() , value.c_str() );
        }
        else
            return au::str( au::str("%%%ds",length).c_str() , value.c_str() );
    }
    
    
    std::string TableColumn::getValue( std::string value )
    {
        return getAlignedValue( getFormatedValue( value ) );
    }
    
    std::string TableColumn::getTitle(  )
    {
        return getAlignedValue( title ); 
    }
    
    void TableColumn::setLength( DataSet *dataSet )
    {
        length = 0;
        
        std::string title = getTitle();
        if ( title.length() > length )
            length = title.length();
        
        for ( size_t r = 0 ; r < dataSet->getNumRecords() ; r++ )
        {
            std::string formated_value = getFormatedValue( dataSet->getValue( r , field ) );
            if( formated_value.length() > length )
                length = formated_value.length();
        }
    }
   
    
#pragma mark
    
    Table::Table( DataSet * _dataSet )
    {
        dataSet = _dataSet;
    }
    
    Table::~Table()
    {
        for ( size_t c = 0 ;  c < columns.size() ; c++ )
            delete columns[c];
        columns.clear();
    }
    
    void Table::addAllFields()
    {
        std::vector<std::string> fields;
        dataSet->getAllFields( fields );
        
        // Insert a colum per field
        for( size_t i = 0 ; i < fields.size() ; i++ )
            columns.push_back (new TableColumn( fields[i] ) );
        
    }
    
    void Table::add( TableColumn * column)
    {
        columns.push_back(column);
    }
    
    std::string Table::line()
    {
        std::ostringstream output;
        
        output << "+-";
        // Separation line
        for ( size_t c = 0 ; c < columns.size() ; c++)
        {
            for( size_t i = 0 ; i < columns[c]->length ; i++ )
                output << "-";
            
            // Get the formatted output
            if( c != ( columns.size()-1) )
                output << "-+-";
        }
        output << "-+";
        output << "\n";
        
        return output.str();
    }
    
    std::string Table::str()
    {
        // Compute the length for every field
        for ( size_t i = 0 ; i < columns.size() ; i++ )
            columns[i]->setLength( dataSet );
        
        std::ostringstream output;

        // Separation line
        output << line();

        // TITLES
        output << "| ";
        for ( size_t c = 0 ; c < columns.size() ; c++)
            // Get the formatted output
            output << columns[c]->getTitle( ) << " | ";
        output << "\n";
        
        // Separation line
        output << line();
        
        for ( size_t r = 0 ; r < dataSet->getNumRecords() ; r++ )
        {
            output << "| ";
            for ( size_t c = 0 ; c < columns.size() ; c++)
            {
                TableColumn *column = columns[c];

                // Get the value containied in the data set
                std::string value = dataSet->getValue(r, columns[c]->field);

                // Get the formatted output
                output << column->getValue( value ) << " | ";
            }
            output << "\n";
        }
        
        output << line();

        
        return output.str();
    }
    
    
    
    
    
    
    
    
}
