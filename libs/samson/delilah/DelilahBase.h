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
    
    
    class XMLStringHolder
    {
        std::string txt;
        au::Cronometer cronometer;
        
        
        bool ready;
        
    public:
        
        XMLStringHolder()
        {
            ready = false;
        }
        
        void update( std::string _txt )
        {
            txt = _txt;
            cronometer.reset();
            ready = true;
        }
        
        size_t getTime()
        {
            return cronometer.diffTimeInSeconds();
        }
        
        void append( std::ostringstream& output)
        {
            output << txt;
        }
        
        bool isReady()
        {
            return ready;
        }
        
    };
    
    class DelilahBase
    {
        // XML information
        int num_workers;
        XMLStringHolder* controller;
        std::vector<XMLStringHolder*> worker;
        
        // Data collected with monitorization
        // Thread safe access 
        au::Token delilah_base_token;
        
        // Global xml-based information from the system
        std::string xml_info;
        
        // General document with the content of xml_info
        pugi::xml_document doc;
        
    public:
        
        DelilahBase( int num_workers );
        ~DelilahBase();
        
        void updateControllerXMLString( std::string txt );
        void updateWorkerXMLString( int w, std::string txt );

        // Own funciton to get xml content
        virtual void getInfo( std::ostringstream& output )=0; 
        
        // Functions to recover content of the xml
        std::string xmlString( int limit );
        std::string xmlString( );

        // Get the time for update
        int getUpdateSeconds();

        // Simple queries to get a list of operations and queues
        std::vector<std::string> getOperationNames( );        
        std::vector<std::string> getQueueNames();
        std::vector<std::string> getDataSetsNames();

        // Advance query interface
        std::string infoCommand( std::string command );        
        std::string getStringInfo( std::string path , node_to_string_function _node_to_string_function  , int options );
        std::string getQuery( std::string query , int limit );

        au::DataSet* getDataSet( std::string command );
        
        au::TreeItem* getTreeItem( );

        std::string updateTimeString();
        
        bool readyForQuery()
        {
            if( !controller->isReady() )
                return false;
            for ( int i=0;i<num_workers;i++)
                if( ! worker[i]->isReady() )
                    return false;
            return true;
            
        }
        
    private:
        
        // Internal command to update the global xml document
        void _reviewXML(  );        
        
        
        std::string _infoCommand( std::string prefix ,  std::string command );
        bool _checkUpdateTime( std::ostringstream &output );
        int _getUpdateSeconds();
        au::DataSet* _getDataSet( std::string command );

        
    };
}


#endif
    