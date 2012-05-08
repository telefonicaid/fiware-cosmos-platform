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
        
        class Item
        {
            // Mutex protextion
            au::Token token;

            // Name describing this item
            std::string name;
            
            // Collection of connections
            int next_id;
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
            
            // Bool removing
            bool removing;
            
        public:

            Item ( Channel * _channel , ConnectionType _type , std::string _name );
            virtual ~Item(){}
            
            std::string getName()
            {
                return name;
            }
            
            void add( Connection* Connection );
            
            void push( engine::Buffer * buffer );

            void review();

            int getNumConnections();
            
            ConnectionType getType()
            {
                return type;
            }
            
            const char* getTypeStr()
            {
                return str_ConnectionType(type);
            }
            
            // Review connections ( overload in specific items )
            virtual void review_item(){}
            
            // Get information about status
            virtual std::string getStatus() { return ""; }
            
            // Check if if it is possible to remove
            virtual bool canBeRemoved()=0;

            
            // Set removing
            void set_removing();
            
            // Check if we are in removing state
            bool isRemoving();
            
            
        };

    }
}

#endif