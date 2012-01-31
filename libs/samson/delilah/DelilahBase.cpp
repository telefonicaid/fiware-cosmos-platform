
/* ****************************************************************************
 *
 * FILE                     DelilahBase.cpp
 *
 * DESCRIPTION			   Basic stuff for delilah client
 *
 */

#include "au/TokenTaker.h"      // au::TokenTaker
#include "au/xml.h"             // au::xml...
#include "au/StringVector.h"

#include "tables/Select.h"

#include "DelilahBase.h"        // Own interface

#define CREATE_TABLE_QUEUES     \
"table_from_tree workers /*//stream_manager/queues/queue -save queues "

#define CREATE_TABLE_OPERATIONS \
"table_from_tree workers /*//modules_manager/modules/module/operations/operation -save operations"

#define CREATE_TABLE_MODULES    \
"table_from_tree workers /*//modules_manager/modules/module -save modules"

#define CREATE_TABLE_ENGINES    \
"table_from_tree workers /*/engine_system -save engines"

#define CREATE_TABLE_ENGINE_DELILAH    \
"table_from_tree delilah /engine_system -save engine_delilah"

#define CREATE_TABLE_STREAM_OPERATIONS \
"table_from_tree workers /*/stream_manager/stream_operations/stream_operation -save stream_operations"

#define CREATE_TABLE_BLOCKS \
"table_from_tree  workers /*//block_manager/blocks/block -save blocks"

namespace samson {    
    
    DelilahBase::DelilahBase(int _num_workers) 
    {
        // Keep the number of workers
        num_workers = _num_workers;
        
        for (int w=0;w<num_workers;w++)
            worker.push_back( new XMLStringHolder() );
    }

    DelilahBase::~DelilahBase()
    {
        for (int w=0;w< num_workers;w++)
            delete worker[w];
        
        worker.clear(); // Clear the vector
    }

    
    
    std::vector<std::string> DelilahBase::getOperationNames(  )
    {
        au::StringVector values = database.getValuesFromColumn("operations", "name");
        values.unique();
        return values;
    }

    std::vector<std::string> DelilahBase::getOperationNames( std::string type  )
    {
        au::tables::SelectCondition condition( "type" , type );
        au::StringVector values = database.getValuesFromColumn("operations", "name" , &condition );
        values.unique();
        return values;
    }
    
    std::vector<std::string> DelilahBase::getDataSetsNames()
    {
        // To be eliminated when controller disapear...
        //return pugi::values( doc , "//controller//queue/name" );
        return std::vector<std::string>();
    }

    std::vector<std::string> DelilahBase::getQueueNames()
    {
        au::StringVector values = database.getValuesFromColumn("queues", "name");
        values.unique();
        return values;
    }
    
    
    void DelilahBase::updateWorkerXMLString( int w, std::string txt )
    {
        if( ( w < 0 ) || ( w >= num_workers))
        {
            LM_W(("Wrong worker id at updateWorkerXMLString (%d)",w));
            return;
        }
        au::tables::TreeItem* tree = worker[w]->update( txt );

        // Set the main name of this tree
        tree->setValue( au::str("worker_%d",w) );

        // Add some markers
        tree->add("tag_name", "worker_id");
        tree->add("tag_value", au::str("%d",w));

        // Replace node int he workers tree
        database.replaceNodeInTree( "workers" , tree );
        
        // Run basic commands to get the table
        database.runCommand( CREATE_TABLE_QUEUES );
        database.runCommand( CREATE_TABLE_OPERATIONS );
        database.runCommand( CREATE_TABLE_MODULES );
        database.runCommand( CREATE_TABLE_ENGINES );
        database.runCommand( CREATE_TABLE_ENGINE_DELILAH );
        database.runCommand( CREATE_TABLE_STREAM_OPERATIONS );
        database.runCommand( CREATE_TABLE_BLOCKS );
    }

    void DelilahBase::updateDelilahXMLString( std::string txt )
    {
        au::tables::TreeItem* tree = delilah->update( txt );
        database.addTree("delilah", tree);
    }
   
    
    std::string DelilahBase::runDatabaseCommand( std::string command)
    {
        return database.runCommand( command );
    }
    
    // Autocomplete for database mode
    void DelilahBase::autoCompleteForDatabaseCommand( au::ConsoleAutoComplete* info )
    {
        database.autoComplete( info );
    }
    
    std::string DelilahBase::updateTimeString()
    {
        std::ostringstream output;
     
        output << "Update times from SAMSON elements\n";
        output << "-------------------------------------------------\n\n";
        
        for (int i = 0 ; i < num_workers ; i++ )
            output << "Worker " << au::str("%2d",i) << "  updated " << au::time_string( worker[i]->getTime() ) << "\n";

        output << "-------------------------------------------------\n";
        
        return output.str();
    }
    
    int DelilahBase::getUpdateSeconds()
    {
        // Get the 
        size_t time = 0;

        for (int w = 0 ; w < num_workers ; w++ )
        {
            size_t _time = worker[w]->getTime();
            if (_time > time )
                time = _time;
        }
        
        return (int) time;
    }    
    
}


