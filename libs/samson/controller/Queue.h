
#ifndef _H_QUEUE
#define _H_QUEUE

#include "samson/common/coding.h"						// samson::KVInfo
#include "MonitorParameter.h"			// samson::MonitorBlock
#include <list>							// std::list
#include "samson/module/KVFormat.h"			// KVFormat
#include "samson/common/samson.pb.h"					// samson::network::...
#include <vector>						// std::vector
#include <iostream>						// std::cout
#include "au/Lock.h"						// au::Lock
#include "au/map.h"						// au::map

namespace samson {
	
	class QueueFile;
	class Queue;
    
	/**
	 Information contained in the controller about a queue
	 */
	
	class Queue
	{
		std::string _name;				// Name of this queue
		KVFormat _format;				// Format of the queue

		// Global information
		FullKVInfo _info;				// Information about this queue
		int _num_files;					// Thread safe number of files ( only for monitoring )

		au::map< std::string , QueueFile > files;		// Map of files included in this queue
		
		
		/**
		 Monitoring system
		 */
		
		friend class Monitor;
		friend class ActiveTask;
		MonitorBlock monitor;							// Set of parameters to monitor for this queue

		friend class ControllerDataManager;

		
	public:
		
		Queue( std::string name , KVFormat format );
		
		~Queue();
		
		std::string getName(){ return _name; }
		KVFormat format() { return _format; }
		FullKVInfo info() { return _info; }

		/**
		 Main functions to add files to this queue
		 */
		void addFile( int worker, std::string _fileName , KVInfo info );


		/*
		 Function to remove an existing file. It return true if the file exists
		 */
		
		bool removeFile( int worker, std::string _fileName , KVInfo info );
		
		
		/**
		 Remove all files
		 */
		
		void clear();
		
		
		/**
		 Copy files from another queu
		 */
		
		void copyFileFrom( Queue *q);
		
		/**
		 Change the name ( it is managed by ControllerDataManager to be coherent
		 */
		
		void rename( std::string name );
				
		/**
		 Get all the information of this queue in "GPB" way
		 */
		

		void fill( network::FullQueue *fq );
        
		
		
		std::string getStatus()
		{
			std::ostringstream o;
			o << _name << "(" << _format.str() << ") [ " << files.size() << " files ] " << _info.str();
			return o.str();		
		}
		
		
		QueueMonitorInfo getQueueMonitorInfo()
		{
			QueueMonitorInfo tmp;
			
			tmp.queue = _name;
			tmp.format = _format.str();
			tmp.info = _info;
			tmp.num_files = _num_files;
			
			return tmp;
		}
	
		const std::string& name()
		{
			return _name;
		}
		
	};
}

#endif