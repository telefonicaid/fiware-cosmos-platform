
#ifndef _H_AUTOMATIC_OPERATION
#define _H_AUTOMATIC_OPERATION

#include "coding.h"						// ss::KVInfo
#include "MonitorParameter.h"			// ss::MonitorBlock
#include <list>							// std::list
#include "samson/KVFormat.h"			// KVFormat
#include "samson.pb.h"					// ss::network::...
#include <vector>						// std::vector
#include <iostream>						// std::cout
#include "Lock.h"						// au::Lock

namespace ss {
	
	class QueueFile;
	class Queue;
	
	
	class AOThrigger
	{
		
	public:
		
		virtual bool thrigger()=0;		// Return true if this trigger ir ready for run
		virtual std::string str()=0;	// Get information about the thrigger
	};
	

	// Simple timeout thrigger
	
	class AOTimeoutThrigger : public AOThrigger
	{
		
	public:
		bool thrigger()
		{
			return false;
		}
		
	};
	
	class AOQueueThrigger : public AOThrigger
	{
		size_t min_size;		// Minimum size to thrigger
		size_t min_num_kvs;		// Minimum number of key-values to thrigger
		Queue *queue;			// Queue we are monitoring
		
	public:
		
		AOQueueThrigger( Queue* _queue , size_t _min_size , size_t _min_num_kvs )
		{
			assert(_queue);
			queue = _queue;
			min_size = _min_size;
			min_num_kvs = _min_num_kvs;
		}
		
		bool thrigger();
		std::string str();
		
	};
	
	
	class AutomaticOperation
	{
		
	public:
				
		std::string command;							// Command to run
		std::vector< AOThrigger* > thriggers;			// Vector of thrigger to run this operation

		// Runtime information about this operation
		bool running;	// Information about if it is running ( not possible to thrigger )
		
		// Information about last execution
		bool last_error;
		std::string last_error_message;
		
		// Tags for this automatic operation
		std::vector<std::string> tags;	

		AutomaticOperation( std::string _command)
		{
			command = _command;	// Copy the command
			
			running = false;
			last_error = false;
		}
		
		~AutomaticOperation()
		{
			for (size_t i = 0 ; i < thriggers.size() ; i++)
				delete thriggers[i];
			
			thriggers.clear();
		}
		
		void add( AOThrigger * t)
		{
			assert(t);
			thriggers.push_back( t );
		}
		
		void addTag( std::string t)
		{
			tags.push_back( t );
		}
		
		bool thrigger( )
		{
			
			if( running )
				return false;		// Never thigger when running...
			
			
			for (size_t i = 0 ; i < thriggers.size() ; i++)
			{
				AOThrigger *t = thriggers[i];
				if( t->thrigger() )
					return true;
			}
			
			return false;
		}
		
		std::string run( )
		{
			running = true;
			return command;
		}
		
		void finish ( bool error, std::string error_message )
		{
			assert( running );
			running = false;
			
			last_error = error;
			last_error_message = error_message;
		}
			
		
		void fill( network::AutomaticOperation *ao )
		{
			ao->set_command( command );

			std::ostringstream t;
			
			if( running ) 
				t << "[R] ";
			
			for ( size_t i  =0  ; i < tags.size() ; i++)
				t << "<" << tags[i] << ">";
			
			for ( size_t i  =0  ; i < thriggers.size() ; i++)
			{
				AOThrigger * th = thriggers[i];
				t << th->str();
			}
			
			ao->set_thrigger( t.str() );
		}
		
		bool checkTag( std::string t )
		{
			for (size_t i = 0 ; i < tags.size() ; i++)
				if( tags[i] == t)
					return true;
			return false;
		}
		
	};
	
}

#endif