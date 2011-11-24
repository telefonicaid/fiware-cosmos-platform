
/* ****************************************************************************
 *
 * FILE                     DelilahBase.cpp
 *
 * DESCRIPTION			   Basic stuff for delilah client
 *
 */

#include "au/TokenTaker.h"      // au::TokenTaker
#include "au/xml.h"             // au::xml...

#include "DelilahBase.h"        // Own interface

namespace samson {
    
    
    DelilahBase::DelilahBase(int _num_workers) : delilah_base_token("DelilahMonitorizationData")
    {
        // Keep the number of workers
        num_workers = _num_workers;
        
        controller = new XMLStringHolder();
        for (int w=0;w<num_workers;w++)
            worker.push_back( new XMLStringHolder() );
    }

    DelilahBase::~DelilahBase()
    {
        delete controller;
        for (int w=0;w< num_workers;w++)
            delete worker[w];
        
        worker.clear(); // Clear the vector
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
    
    void DelilahBase::updateControllerXMLString( std::string txt )
    {
        // thread protection
        au::TokenTaker tt( &delilah_base_token  );
        
        controller->update( txt );
        _reviewXML();
    }
    
    void DelilahBase::updateWorkerXMLString( int w, std::string txt )
    {
        // thread protection
        au::TokenTaker tt( &delilah_base_token  );
        
        if( ( w < 0 ) || ( w >= num_workers))
        {
            LM_W(("Wrong worker id..."));
            return;
        }
        worker[w]->update( txt );
        _reviewXML();
    }
   
    
    void DelilahBase::_reviewXML(  )
    {
        
        std::ostringstream output;
        
        output << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
        output << "<samson>\n";

        output << "<controller>";
        controller->append(output);
        output << "</controller>";
        
        
        for (int i = 0 ; i < num_workers ; i++ )
        {
            output << "<worker>";
            au::xml_simple( output , "id" , i );
            worker[i]->append( output );
            output << "</worker>";
        }
        
        // Get my own status here ( delilah )
        output << "<delilah>";
        getInfo( output );
        output << "</delilah>";
        

        output << "</samson>\n";
        
        // Set the txt for debuggin
        xml_info = output.str();
        
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
        cmdLine.set_flag_boolean("small_title");
        cmdLine.parse( command );
        
        std::ostringstream output;

        // Check up time
        if ( !_checkUpdateTime(output) )
            return output.str();
        
        // CONTROLLER
        if( cmdLine.get_flag_bool("controller") )
        {
            if ( cmdLine.get_flag_bool("small_title") )
                output << "Controller :\n";
            else if( !cmdLine.get_flag_bool("no_title") )
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
                
                if ( cmdLine.get_flag_bool("small_title") )
                    output << "Worker " << workers_ids[w] << ":\n";
                else if( !cmdLine.get_flag_bool("no_title") )
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
            if ( cmdLine.get_flag_bool("small_title") )
                output << "Delilah :\n";
            else if( !cmdLine.get_flag_bool("no_title") )
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
    
    au::TreeItem* DelilahBase::getTreeItem( )
    {
        au::TokenTaker tt( &delilah_base_token  );
        return pugi::treeItemFromDocument(doc);
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

            output << filter.str( &dataSet );
/*            
            au::DataSet* outputDataSet = filter.transform( &dataSet);
            output << outputDataSet->str();
            delete outputDataSet;
   */         
            
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
            output << au::str( "Error: Monitor information is %d seconds old" , time );
            return false;
        }
        
        if( time > soft_limit )
            output <<  au::str( "Warning: Monitor information is %d seconds old\n" , time );
        
        return true;
        
    }
    
    std::string DelilahBase::updateTimeString()
    {
        std::ostringstream output;
     
        output << "Update times from SAMSON elements\n";
        output << "-------------------------------------------------\n\n";
        
        output << "Controller updated " << au::time_string( controller->getTime() ) << "\n";
        for (int i = 0 ; i < num_workers ; i++ )
            output << "Worker " << au::str("%2d",i) << "  updated " << au::time_string( worker[i]->getTime() ) << "\n";

        output << "-------------------------------------------------\n";
        
        return output.str();
    }
    
    int DelilahBase::_getUpdateSeconds()
    {
        
        // Get the 
        size_t time = controller->getTime();
        

        for (int w = 0 ; w < num_workers ; w++ )
        {
            size_t _time = worker[w]->getTime();
            if (_time > time )
                time = _time;
        }
        
        return (int) time;
    }    
    
}


