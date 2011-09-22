
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
        
    public:
        
        ~DataSetRow();
        
        void set( std::string field , std::string value );
        int length( std::string field );
        void getAllFields( std::set<std::string> &fields );

        std::string get( std::string field );
        
    };

    class DataSet
    {
        std::vector< DataSetRow* > rows;
        
    public:
        
        ~DataSet();
        
        void add( DataSetRow* row);
        
        void getAllFields( std::vector<std::string> &fields );
        
        size_t getNumRecords();
        std::string getValue( int record , std::string field);
    
    };
    
    class TableColumn
    {
        std::string title;      // title for this column
        std::string field;      // Field
        size_t length;          // Max length in this columnd
        
        bool left;              // Left aligned?
        std::string format;     // Particular format
        
        friend class Table;
        
    public:
        
        TableColumn( std::string field_definition );

        std::string getValue( std::string value );
        std::string getTitle(  );

        void setLength( DataSet *dataSet );
        
    private:
        
        void processModifier( std::string modifier );

        std::string getFormatedValue( std::string value );
        std::string getAlignedValue( std::string value );
    };
    
    class Table
    {
        DataSet * dataSet;
        
        std::vector<TableColumn*> columns;
        
    public:
        
        Table( DataSet * _dataSet );
        ~Table();
        
        void addAllFields();
        void add( TableColumn * column);
        
        std::string str();
              
    private:
        
        std::string line();
        
    };
    
}


#endif