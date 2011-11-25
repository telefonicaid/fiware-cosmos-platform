
/* ****************************************************************************
 *
 * FILE            Table.h
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         au library
 *
 * DATE            Septembre 2011
 *
 * DESCRIPTION
 *
 *      Usefull classes to draw tabulated data on screen
 *
 * COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/

#ifndef _H_AU_TABLE
#define _H_AU_TABLE

#include <stdio.h>             /* sprintf */
#include <string>				// std::string
#include <sys/time.h>           // struct timeval
#include <sstream>              // std::ostringstream
#include "logMsg/logMsg.h"             // LM_W

#include "au/CommandLine.h"
#include "au/ErrorManager.h"        // au::ErrorManager
#include "au/map.h"                 // au::map
#include "au/string.h"              // au::str(...)

namespace au {
    
    
    
    // Conversion function
    double toDouble( std::string value );
    std::string toString( double value );

    class DataSetRow
    {
        au::map< std::string , std::string > fields;
        
        friend class DataSetFilter;
        
    public:
        
        ~DataSetRow();
        
        void set( std::string field , std::string value );
        int getLength( std::string field );
        void getAllFields( std::set<std::string> &fields );
        std::string get( std::string field );
        
    };

    class DataSet
    {
        std::vector< DataSetRow* > rows;
     
        friend class DataSetFilter;
        
    public:
        
        ~DataSet();
        
        void add( DataSetRow* row);
        void getAllFields( std::vector<std::string> &fields );
        
        size_t getNumRecords();
        std::string getValue( int record , std::string field);
        size_t getMaxLength( std::string field , bool title_considered );
        std::string str();

        // Set to all the records
        void set( std::string field , std::string value );
       
        void add( DataSet* set )
        {
            rows.insert( rows.end() , set->rows.begin(), set->rows.end() );
        }
        
        
        
    };
    
    class DataSetFilterColumn
    {
        std::string title;      // title for this column
        std::string field;      // Field

        
        bool left;              // Left aligned?
        std::string format;     // Particular format
        
        friend class DataSetFilter;
        
    public:
        
        DataSetFilterColumn( std::string field_definition );

        std::string getValue( std::string value , size_t length );
        std::string getTitle( size_t length );

        
    private:
        
        void processModifier( std::string modifier );

        std::string getFormatedValue( std::string value );
        std::string getAlignedValue( std::string value,size_t length );
    };
    
    class DataSetFilter
    {
        
        std::vector<DataSetFilterColumn*> columns;
        
    public:
        
        DataSetFilter();
        DataSetFilter( std::string definition);
        ~DataSetFilter();
        
        void addAllFields( DataSet * dataSet );
        void add( DataSetFilterColumn* column );

        DataSet* transform( DataSet* input );
        
        std::string str( DataSet* input );
        std::string str( DataSet* input , int limit );
        
        
    };
    
}


#endif