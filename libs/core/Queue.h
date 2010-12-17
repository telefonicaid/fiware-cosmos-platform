
#ifndef _H_QUEUE
#define _H_QUEUE

#include "coding.h"						// ss::KVInfo
#include "MonitorParameter.h"			// ss::MonitorBlock
#include <list>							// std::list
#include "samson/KVFormat.h"			// KVFormat
#include "samson.pb.h"					// ss::network::...

namespace ss {
	
	class QueueFile;
	
	/**
	 Information contained in the controller about a queue
	 */
	
	class Queue
	{
		std::string _name;				// Name of this queue
		KVFormat _format;				// Format of the queue
		KVInfo _info;					// Information about this queue

		/**
		 Monitoring system
		 */
		
		friend class Monitor;
		MonitorBlock monitor;			// Set of parameters to monitor for this queue

		friend class ControllerDataManager;
		std::list< QueueFile* > files;	// List of files
		int _num_files;					// Thread safe number of files ( only for monitoring )

	public:
		
		Queue( std::string name , KVFormat format )
		{
			_name = name;
			_format = format;
			
			monitor.addMainParameter( "name" , _name );
			monitor.addMainParameter( "format" , _format.str() );

			_num_files =0 ;
			
		}
		
		std::string getName(){ return _name; }
		KVFormat format() { return _format; }
		KVInfo info() { return _info; }

		/**
		 Main functions to add files to this queue
		 */
		void addFile( int worker, std::string _fileName , KVInfo info );
		
		
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
		 Insert files in another place
		 */
		 
		void insertFilesIn( network::FileList &fileList);
		
		std::string getStatus()
		{
			std::ostringstream o;
			o << _name << "(" << _format.str() << ") [ " << files.size() << " files ] " << _info.str();
			return o.str();		
		}
		
		
		void takeMonitorSamples()
		{
			monitor.push( "size"	, _info.size );
			monitor.push( "num_kvs" , _info.kvs );
			monitor.push( "files"	, _num_files );
		}
		
	};
}

#endif