
#ifndef _H_SAMSON_STREAM_STATE
#define _H_SAMSON_STREAM_STATE

#include <string>           // std::string

#include "au/list.h"        // au::list

#include "samson/common/coding.h"       // KVFullInfo
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
            
            // Flag to avoid creating new operations ( necessary for removing a state )
            bool paused;
            
        public:
            
            State( std::string _name );
            ~State();
            
            void push( BlockList *list );
            
            // Function to check if any of the items is working ( if so, it can not be removed )
            bool isWorking();
            
            
        public:
            
            // Getting XML information
            void getInfo( std::ostringstream& output);
            
        private:
            
            void divide( StateItem *item , StateItem *item1 , StateItem *item2 );

            
            // Get information about the state and input
            void copyStateList( BlockList * list  );
            void copyInputList( BlockList * list  );
            
        };
        
        
    }
}

#endif