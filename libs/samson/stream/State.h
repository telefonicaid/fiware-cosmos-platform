
#ifndef _H_SAMSON_STREAM_STATE
#define _H_SAMSON_STREAM_STATE

#include <string>           // std::string

#include "au/list.h"        // au::list

#include "samson/common/samson.pb.h"    // samson::network::...

namespace samson {
    
    namespace stream
    {

        class StateItem;
        class BlockList;
        class ReduceQueueTask;
        
        class State
        {
            
            friend class StateItem;
            friend class QueuesManager;
            std::string name;

            // List of state items in this state
            au::list< StateItem > items;
            
        public:
            
            State( std::string _name );
            ~State();
            
            void push( BlockList *list );
                        
            // Get Information in XML format
            void getInfo( std::ostringstream& output);

            std::string getStatus();
            
            
        private:
            
            void divide( StateItem *item , StateItem *item1 , StateItem *item2 );
            
        };
        
        
    }
}

#endif