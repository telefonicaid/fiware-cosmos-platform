#ifndef _H_CONTROLLER_DATA_MANAGER
#define _H_CONTROLLER_DATA_MANAGER

#include "Lock.h"				// au::Lock
#include <sstream>				// std::ostringstream
#include "samson/KVFormat.h"	// ss:: KVFormat
#include "Format.h"				// au::Format
#include <map>					// std::map
#include "DataManager.h"		// ss::DataManager
#include "au_map.h"				// au::map
#include "samson.pb.h"			// ss::network::...
#include "MonitorParameter.h"	// ss::MonitorBlock
#include "coding.h"				// FileKVInfo

namespace ss {
	
	class Queue;
	class ControllerTask;
	class DataManagerCommandResponse;
	class SamsonController;
	class ControllerTaskInfo;

	/**
	 Data manager at the controller
	 */
	
	class ControllerDataManager : public DataManager
	{
		//au::Lock lock;
		
		au::map< std::string , Queue> queues;			// List of KeyValue queues
		
		SamsonController *controller;					// Pointer to controller for module access
		
		FileKVInfo info_kvs;								// Global info of the entire system ( all types of key-values )
		FileKVInfo info_txt;								// Global info of the entire system ( txt files )
		
	public:
		
		ControllerDataManager( SamsonController *_controller ) : DataManager( getLogFileName()  )
		{
			controller = _controller;
			info_kvs.init();
			info_txt.init();
		}
		
		/**
		 Get a string describing status
		 Usefull for debuggin
		 */
		
		std::string getStatus();

		/**
		 Get the fileName of the log file
		 */
		
		static std::string getLogFileName( );

		// Get help about data stuff
		void helpQueues( network::HelpResponse *response , network::Help help );

		void fill( network::QueueList *ql , std::string command);

		void retreveInfoForTask( ControllerTaskInfo *info );		
		
		static std::string getAddFileCommand(int worker , std::string fileName , size_t size , size_t kvs , std::string queue )
		{
			std::ostringstream command;
			command << "add_file " <<  worker << " " << fileName << " " << size << " " << kvs << " " << queue;
			return command.str();
		}
		
		
		FileKVInfo get_info_kvs()
		{
			return info_kvs;
		}
		
		FileKVInfo get_info_txt()
		{
			return info_txt;
		}
		
	private:

		
		void _retreveInfoForTask( ControllerTaskInfo *info );		
		
		virtual DataManagerCommandResponse _run( std::string command );
		virtual void _un_run( std::string command );
		
		
		
	};

}

#endif
