
/* ****************************************************************************
 *
 * FILE                     DelilahBase.cpp
 *
 * DESCRIPTION			   Basic stuff for delilah client
 *
 */

#include "au/TokenTaker.h"      // au::TokenTaker

#include "DelilahBase.h"        // Own interface

namespace samson {
    
    
    DelilahBase::DelilahBase() : delilah_base_token("DelilahMonitorizationData")
    {
    }
    
    std::string DelilahBase::xmlString( int limit )
    {
        // thread protection
        au::TokenTaker tt( &delilah_base_token  );
        return pugi::str( doc  , limit );
    }
    
    std::string  DelilahBase::xmlString( )
    {
        au::TokenTaker tt( &delilah_base_token  );
        return xml_info;
    }
    
    std::vector<std::string> DelilahBase::getOperationNames(  )
    {
        // thread protection
        au::TokenTaker tt( &delilah_base_token  );
        return pugi::values( doc , "//controller//operation/name" );
    }
    
    std::vector<std::string> DelilahBase::getQueueNames()
    {
        au::TokenTaker tt( &delilah_base_token  );
        return pugi::values( doc , "//controller//queue/name" );
    }
    
   
    
    void DelilahBase::updateXMLString( std::string txt )
    {
        // thread protection
        au::TokenTaker tt( &delilah_base_token  );
        
        // Set the txt for debuggin
        xml_info = txt;
        
        // Reset the cronometer
        cronometer_xml_info.reset();
        
        // Parser the xml document into "doc"
        doc.reset();
        std::istringstream is_xml_document( xml_info );
        pugi::xml_parse_result result = doc.load( is_xml_document );
    }
    
    int DelilahBase::getUpdateSeconds()
    {
        // thread protection
        au::TokenTaker tt( &delilah_base_token  );
        
        return _getUpdateSeconds();
    }

    std::string DelilahBase::getQuery( std::string query , int limit )
    {
        // thread protection
        au::TokenTaker tt( &delilah_base_token  );
        
        pugi::xpath_node_set result;
        try {
            result = doc.select_nodes( query.c_str() );
        } catch (pugi::xpath_exception& ex) 
        {
            return au::str( "Error in xpath query: %s" , ex.what() );
            return 0;
        }
        
        // Transform the results into a string
        std::ostringstream result_txt;
        pugi::str( result , result_txt , limit ); 
        
        return result_txt.str();
    }    
    
    
    std::string DelilahBase::getStringInfo( std::string path , node_to_string_function _node_to_string_function  , int options )
    {
        // thread protection
        au::TokenTaker tt( &delilah_base_token  );

        std::ostringstream output;
        
        // Check up time
        if ( !_checkUpdateTime(output) )
            return output.str();
        
        if( options & i_controller )
        {
            if( !(options & i_no_title ) )
            {
                output << "\n";
                output << "================================================================================\n";
                output << "Controller :\n";
                output << "================================================================================\n";
                output << "\n";
            }
            
            pugi::xpath_node_set nodes  = pugi::select_nodes( doc , "//controller" + path );
            
            for ( size_t i = 0 ; i < nodes.size() ; i++ )
            {
                const pugi::xml_node& node = nodes[i].node(); 
                output << _node_to_string_function( node ) << "\n" ;
            }      
        }
        
        if( options & i_worker )
        {
            pugi::ValuesCollection workers_ids = pugi::values(doc, "//worker/id");
            
            for ( size_t w = 0 ; w < workers_ids.size() ; w++ )
            {
                
                if( !(options & i_no_title ) )
                {
                    output << "\n";
                    output << "================================================================================\n";
                    output << "Worker " << workers_ids[w] << ":\n";
                    output << "================================================================================\n";
                    output << "\n";
                }
                
                pugi::xpath_node_set nodes  = pugi::select_nodes( doc , "//worker[id=" + workers_ids[w] + "]" + path );
                
                for ( size_t i = 0 ; i < nodes.size() ; i++ )
                {
                    const pugi::xml_node& node = nodes[i].node(); 
                    output << _node_to_string_function( node ) << "\n" ;
                }            
                
            }
        }
        
        
        if( options & i_delilah )
        {
            if( !(options & i_no_title ) )
            {
                output << "\n";
                output << "================================================================================\n";
                output << "Delilah :\n";
                output << "================================================================================\n";
                output << "\n";
            }
            
            pugi::xpath_node_set nodes  = pugi::select_nodes( doc , "//delilah" + path );
            
            for ( size_t i = 0 ; i < nodes.size() ; i++ )
            {
                const pugi::xml_node& node = nodes[i].node(); 
                output << _node_to_string_function( node ) << "\n" ;
            }            
        }
        
        return output.str();
        
    }
    
    std::string DelilahBase::infoCommand( std::string command )
    {
        // thread protection
        au::TokenTaker tt( &delilah_base_token  );

        
        au::CommandLine cmdLine;
        cmdLine.set_flag_boolean("controller");
        cmdLine.set_flag_boolean("worker");
        cmdLine.set_flag_boolean("delilah");
        cmdLine.set_flag_boolean("no_title");
        cmdLine.parse( command );
        
        std::ostringstream output;

        // Check up time
        if ( !_checkUpdateTime(output) )
            return output.str();
        
        // CONTROLLER
        if( cmdLine.get_flag_bool("controller") )
        {
            if( !cmdLine.get_flag_bool("no_title") )
            {
                output << "\n";
                output << "================================================================================\n";
                output << "Controller :\n";
                output << "================================================================================\n";
                output << "\n";
            }
            
            output << _infoCommand("//controller" , command );
        }
        
        
        // WORKERS
        if( cmdLine.get_flag_bool("worker") )
        {
            pugi::ValuesCollection workers_ids = pugi::values(doc, "//worker/id");
            
            for ( size_t w = 0 ; w < workers_ids.size() ; w++ )
            {
                
                if( !cmdLine.get_flag_bool("no_title") )
                {
                    output << "\n";
                    output << "================================================================================\n";
                    output << "Worker " << workers_ids[w] << ":\n";
                    output << "================================================================================\n";
                    output << "\n";
                }
                
                output << _infoCommand( "//worker[id=" + workers_ids[w] + "]" , command );
                
            }
        }
        
        // DELILAH
        if( cmdLine.get_flag_bool("delilah") )
        {
            if( !cmdLine.get_flag_bool("no_title") )
            {
                output << "\n";
                output << "================================================================================\n";
                output << "Delilah :\n";
                output << "================================================================================\n";
                output << "\n";
            }
            
            output << _infoCommand("//delilah" , command );
        }

        
        return output.str();
        
    }
    
    au::DataSet* DelilahBase::getDataSet( std::string command )
    {
        // thread protection
        au::TokenTaker tt( &delilah_base_token  );
        
        return _getDataSet(command);
    }
    
    au::DataSet* DelilahBase::_getDataSet( std::string command )
    {
        
        au::DataSet *dataSet = new au::DataSet();
        
        
        // Controller
        {
            au::DataSet *_dataSet = new au::DataSet();
            pugi::xpath_node_set nodes  = pugi::select_nodes( doc , "//controller" + command );
            pugi::dataSetFromNodes( *_dataSet , nodes );
            _dataSet->set("node","controller");
            dataSet->add( _dataSet );
        }
        
        
        pugi::ValuesCollection workers_ids = pugi::values(doc, "//worker/id");
        for ( size_t w = 0 ; w < workers_ids.size() ; w++ )
        {
            au::DataSet *_dataSet = new au::DataSet();
            pugi::xpath_node_set nodes  = pugi::select_nodes( doc , "//worker[id=" + workers_ids[w] + "]" + command );
            pugi::dataSetFromNodes( *_dataSet , nodes );
            _dataSet->set("node","worker");
            _dataSet->set("node_id" , au::str("%d" , (int) w) );
            dataSet->add( _dataSet );
        }
        
        
        // Delilah
        {
            au::DataSet *_dataSet = new au::DataSet();
            pugi::xpath_node_set nodes  = pugi::select_nodes( doc , "//delilah" + command );
            pugi::dataSetFromNodes( *_dataSet , nodes );
            _dataSet->set("node","delilah");
            dataSet->add( _dataSet );
        }
        
        
        
        
        return dataSet;
    }
    
    std::string DelilahBase::_infoCommand( std::string prefix ,  std::string command )
    {
        // Private command only called from infoCommand ( no therad protection required )
        std::ostringstream output;

        // Check up time
        if ( !_checkUpdateTime(output) )
            return output.str();

        // Parse input command
        au::CommandLine cmdLine;
        cmdLine.set_flag_boolean("info");
        cmdLine.parse( command );
        
        
        if( cmdLine.get_num_arguments() < 2 )
            return "Usage: info_command select_queue fields [options] [-info]";
        
        std::string select = cmdLine.get_argument(1);
        
        {
            
            // Select nodes
            pugi::xpath_node_set nodes  = pugi::select_nodes( doc , prefix + select );
            
            // Get the data set form nodes
            au::DataSet dataSet;
            pugi::dataSetFromNodes( dataSet , nodes );
            
            // If select is activated. just show the fields
            if( cmdLine.get_flag_bool("info") )
            {
                std::vector<std::string> fields;
                dataSet.getAllFields(fields);
                for ( size_t i = 0 ; i < fields.size() ; i ++)
                    output << "Field: " << fields[i] << "\n";
                
                output << "\n";
                output << "Num records " << dataSet.getNumRecords();
                
                return output.str();
            }
            
            // Table shoing contents
            au::DataSetFilter filter;
            
            //table.addAllFields();
            
            for ( int i = 2 ; i < cmdLine.get_num_arguments() ; i++)
            {
                std::string field_definition = cmdLine.get_argument(i);
                filter.add( new au::DataSetFilterColumn( field_definition ) );
            }
            
            au::DataSet* outputDataSet = filter.transform( &dataSet);
            
            output << outputDataSet->str();
            
            delete outputDataSet;
            
        }
        
        return output.str();
        
    }        
 
    
    bool DelilahBase::_checkUpdateTime( std::ostringstream &output )
    {
        int soft_limit = 10;
        int hard_limit  = 60;
        
        int time = _getUpdateSeconds();
        
        if( time > hard_limit )
        {
            output << au::str( "Error: Monitorization information is %d seconds old" , time );
            return false;
        }
        
        if( time > soft_limit )
            output <<  au::str( "Warning: Monitorization information is %d seconds old\n" , time );
        
        return true;
        
    }
    
    int DelilahBase::_getUpdateSeconds()
    {
        
        // Get the 
        int time = cronometer_xml_info.diffTimeInSeconds();
        
        // Sum the worker time
        // TO BE COMPLETED
        
        return time;
    }    
    
}


