
#ifndef _H_WORKER_COMMAND_DELILAH_COMPONENT
#define _H_WORKER_COMMAND_DELILAH_COMPONENT

#include "au/ErrorManager.h"        // au::ErrorManager
#include "au/Cronometer.h"          // au::CronometerSystem

#include <cstring>


#include "engine/BufferContainer.h"

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
	
    class WorkerResponese
    {
        
    public:
        
        size_t worker_id;
        au::ErrorManager error;
        
        WorkerResponese( size_t _worker_id )
        {
            worker_id = _worker_id;
        }
        
        WorkerResponese( size_t _worker_id , std::string error_message )
        {
            worker_id = _worker_id;
            error.set( error_message );
        }
        
    };
    
	class WorkerCommandDelilahComponent : public DelilahComponent
	{
		std::string command;
        
		engine::BufferContainer buffer_container;

        std::set<size_t> workers;                       // Ids of the workers involved in this command
        au::map<size_t , WorkerResponese > responses;   // Map with all the responses from workers
        
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
        }
		
		void receive( Packet* packet );
		void run();
		
		std::string getStatus();

    private:
    
        void print_content( network::Collection * collection );
        
	};
    
	
	
}

#endif