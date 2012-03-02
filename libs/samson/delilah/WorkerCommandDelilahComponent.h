
#ifndef _H_WORKER_COMMAND_DELILAH_COMPONENT
#define _H_WORKER_COMMAND_DELILAH_COMPONENT

#include "au/ErrorManager.h"        // au::ErrorManager
#include "au/Cronometer.h"          // au::CronometerSystem

#include <cstring>
#include "samson/network/Message.h"		// Message::MessageCode 
#include "samson/network/Packet.h"			// samson::Packet
#include "samson/common/samson.pb.h"	


namespace engine {
    class Buffer;
}

namespace samson {
	
	class Delilah;

	
	/**
	 Simple component created when a command is send to the controller ( waiting for answeres )
	 */
	
	class WorkerCommandDelilahComponent : public DelilahComponent
	{
		std::string command;
		engine::Buffer *buffer;

        std::set<size_t> workers;             // Ids of the workers involved in this command
        std::set<size_t> confirmed_workers;   // Ids of the workers confirmed finish of this operation
        
        // Collections reported by workers
        au::map<std::string, network::Collection > collections;
        
        size_t worker_id;               // if != -1 --> worker to sent this command
        bool save_in_database;          // -save
        std::string group_field;        // -group
        std::string filter_field;       // -filter
        std::string sort_field;         // -sort
        bool connected_workers;         // -connected
        int limit;                      // -limit
        
	public:
		
		WorkerCommandDelilahComponent( std::string _command , engine::Buffer *buffer );
        ~WorkerCommandDelilahComponent()
        {
            if( buffer )
            {
                engine::MemoryManager::shared()->destroyBuffer( buffer );
            }
        }
		
		void receive( Packet* packet );
		void run();
		
		std::string getStatus();
        
    private:
    
        void print_content( network::Collection * collection );
        
	};
    
	
	
}

#endif