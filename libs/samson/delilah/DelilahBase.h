#ifndef _H_DELILAH_BASE
#define _H_DELILAH_BASE

/* ****************************************************************************
 *
 * FILE                     DelilahBase.h
 *
 * DESCRIPTION			   Basic stuff for delilah client
 *
 */


#include <string>                       // std::string
#include <vector>                       // std::vector

#include "au/Token.h"                   // au::Token
#include "au/Cronometer.h"              // au::Cronometer

#include "DelilahUtils.h"               // 

#include "pugi/pugi.h"                  // pugi::... node_to_string_function

namespace samson {
    
    const int i_controller  = 1;    
    const int i_worker      = 1<<1;    
    const int i_delilah     = 1<<2;    
    const int i_no_title    = 1<<3;    
    
    
    /*
     Class to hold monitorization data for delilah components ( delilah / monitorization )
     */
    
    class DelilahBase
    {
        // Data collected with monitorization
        // Thread safe access 
        
        au::Token token;
        
        // Global xml-based information from the system
        std::string xml_info;
        
        // General document with the content of xml_info
        pugi::xml_document doc;
        
        // Cronometer for xml_info update
        au::Cronometer cronometer_xml_info;
        
    public:
        
        DelilahBase();
        
        // Functions to recover content of the xml
        std::string xmlString( int limit );
        std::string xmlString( );

        // Update the XML string
        void updateXMLString( std::string txt );        
        
        // Get the time for update
        int getUpdateSeconds();

        // Simple queries to get a list of operations and queues
        std::vector<std::string> getOperationNames( );        
        std::vector<std::string> getQueueNames();

        // Advance query interface
        std::string infoCommand( std::string command );        
        std::string getStringInfo( std::string path , node_to_string_function _node_to_string_function  , int options );
        std::string getQuery( std::string query , int limit );
        
    private:
        
        std::string _infoCommand( std::string prefix ,  std::string command );
        bool _checkUpdateTime( std::ostringstream &output );
        int _getUpdateSeconds();
        
    };
}


#endif
    