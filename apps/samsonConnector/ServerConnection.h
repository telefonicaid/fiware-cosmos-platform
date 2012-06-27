#ifndef _H_SAMSON_CONNECTOR_SERVER_CONNECTION
#define _H_SAMSON_CONNECTOR_SERVER_CONNECTION

#include <string>

#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"

#include "common.h" 
#include "Item.h" 

namespace samson 
{
    namespace connector
    {
        
        class Channel;
        class SocketConnection;
        class SamsonConnector;

        // Item to stablish a socket connection with a host:port
        
        class ConnectionItem : public Item
        {
            // Information to establish connection
            std::string host;
            int port;

            // Information about retrials
            au::Cronometer connection_cronometer;
            int connection_trials;
            
        public:
            
            // Constructor & Destructor
            ConnectionItem( Channel* _channel , ConnectionType _type , std::string _host , int _port  );

            // Information about status
            std::string getStatus();
            
            // Item methods
            virtual void start_item();
            virtual void review_item();
            virtual void stop_item();
            
        private:
            
            void try_connect();

            
        };
        
        
        // Item to read stdin
        class StdinItem : public Item
        {
            
        public:
            
            // Constructor & Destructor
            StdinItem( Channel* _channel );
            
            // Information about status
            std::string getStatus();
            
            void start_item();
            void review_item();
            void stop_item();
            
            
        };
        
        
        // Item to write on stdout
        class StdoutItem : public Item
        {
            
        public:
            
            // Constructor & Destructor
            StdoutItem( Channel* _channel );
            
            std::string getStatus();
            
            void start_item();
            void review_item();
            void stop_item();
            
            
        };
        
    }
    
    
}


#endif