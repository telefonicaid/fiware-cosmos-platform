#ifndef _H_SAMSON_CONNECTOR_ITEM
#define _H_SAMSON_CONNECTOR_ITEM

#include "au/containers/map.h"

#include "engine/Buffer.h"
#include "engine/BufferContainer.h"
#include "samson/client/SamsonClient.h"

#include "common.h"
#include "TrafficStatistics.h"
#include "BufferProcessor.h"

namespace samson 
{
    namespace connector
    {
  
        class Connection;
        class Channel;
        class InputInterChannelConnection;
        
        class Item
        {
            // Mutex protextion
            au::Token token;

            // Name describing this item
            std::string name_;                   // Assigned when added to a channel
            std::string description_;       // Description of this element
            
            // Collection of connections
            int next_id; // Next identifier
            au::map<int, Connection> connections;
            
            // Statistic information
            TrafficStatistics traffic_statistics;
            
            // Channel pointer
            Channel * channel;
            
            // Type of channel
            ConnectionType type;
            
            friend class Connection;
            friend class Channel;
            friend class SamsonConnector;
                        
            bool canceled; // Flag to indicate this is canceled ( not call review again )
            bool finished; // Flag to indicate this component is finished

        public:

            Item ( Channel * _channel , ConnectionType _type , std::string description );
            virtual ~Item();            

            // Add a connection
            void add( Connection* Connection );

            // Push a new buffer to all connections
            void push( engine::Buffer * buffer );

            // Reviewing item and all associated connections
            void review();

            // Getting information
            int getNumConnections();
            ConnectionType getType();
            std::string getName();
            std::string getFullName();
            std::string getDescription();
            const char* getTypeStr();
            
            // Review connections ( overload in specific items )
            virtual void review_item(){}
            
            // Get information about status
            virtual std::string getStatus() { return ""; }
            
            // Stop all threads to be deleted
            virtual void stop_item(){};

            void init_item()
            {
                start_item();
            };
            
            virtual void start_item(){};
            
            // Cancel this item and all associated connections
            void cancel_item();
          
            // Check if we accept a particular connection
            virtual bool accept( InputInterChannelConnection* connection )
            {
                // By default, we do not accept interchannel connections
                return false;
            }

            // Mark this element as finished ( can be removed by remove_finished_* commands )
            void set_as_finished();
            bool is_finished();

            void remove_finished_connections(au::ErrorManager* error);
          
            void write( au::ErrorManager * error );
            

            // Log system
            void log( std::string type , std::string message );
            void log( Log* log );
            
        };

    }
}

#endif