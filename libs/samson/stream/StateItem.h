

#include <string>           // std::string

#include "engine/Buffer.h"              // engine::Buffer

#include "samson/common/samson.pb.h"    // samson::network::....


namespace samson {
    
    namespace stream
    {
        
        class BlockList;
        class State;
        class Block;
        class ReduceQueueTask;
        
        class StateItem
        {
            
            friend class QueuesManager;
            friend class State;
            
            typedef enum 
            {
                ready,                  // the item is ready to be executed when new input is available
                running,                // We are currently running a task
                running_dividing,       // We are currently running two tasks. After both operation finishes, two items will be created
            } Mode;
            
            State * myState;
            
            
            BlockList* input;            // Block list of pending blocks to be processed
            BlockList* state;           // Block list containing current state

            Mode mode;
            
            BlockList* future_state;     // Block list containing future state
            BlockList* future_state2;    // Block list containing future state

            ReduceQueueTask* task;  // Pointer to the current task reducing input to the state
            ReduceQueueTask* task2;  // Pointer to the current task reducing input to the state
            
        public:
            
            int hg_begin;
            int hg_end;

            
            StateItem( State * _myState,  int _hg_begin , int _hg_end );
            ~StateItem();
            
            // Push input data to be processed
            void push( BlockList *list );

            // Add state ( while running operation )
            void addStateBuffer( ReduceQueueTask* _task , engine::Buffer* buffer);
            void notifyFinishOperation( ReduceQueueTask* _task );
        
            // Get Information in XML format
            void getInfo( std::ostringstream& output);
            
            bool isReadyToRun();

            void setRunning( ReduceQueueTask* task );
            void setRunning( ReduceQueueTask* task , ReduceQueueTask* task2 );
            
            bool isWorking();

            
            FullKVInfo getFullKVInfo();
            
        };
        
    }
}