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
#include "AutomaticOperationManager.h"	// ss::AutomaticOperationManager

namespace ss {
	
	
	class Queue;
	class ControllerTask;
	class DataManagerCommandResponse;
	class SamsonController;
	class ControllerTaskInfo;
	class QueueAutomaticOperation;
	
	class ActiveTask
	{
	public:
		std::set<std::string> files;		// List of files to not delete from disk
		
		void addFiles( Queue *q );			// Add all the files of this queue to the list of protected files
		
	};
	
	/**
	 Data manager at the controller
	 */
	
	class ControllerDataManager : public DataManager
	{
		//au::Lock lock;	(lock is included in DataManager generic object )
		
		FileKVInfo info_kvs;										// Global info of the entire system ( all types of key-values )
		FileKVInfo info_txt;										// Global info of the entire system ( txt files )
		
		au::map< std::string , Queue> queues;						// List of ALL KeyValue queues
		
		au::map< size_t , ActiveTask > tasks;						// List of active tasks ( information about the files that should not be deleted )
		
		AutomaticOperationManager automatic_operations_manager;		// Manager of automatic operations
		
	public:

		ControllerDataManager( ) : DataManager( getLogFileName() )
		{
			info_kvs.clear();
			info_txt.clear();
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
		void fill( network::AutomaticOperationList *aol , std::string command);
		void fill( network::DownloadDataInitResponse* response , std::string queue );
		
		void retreveInfoForTask( size_t job_id , ControllerTaskInfo *info , bool clear_inputs );		
		
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
		
		
		void getQueuesMonitorInfo( std::vector<QueueMonitorInfo> &queues_names );

		
		
		// Automatic operation API to get list of pending jobs and set as finished
		
		std::vector<AOInfo> getNextAutomaticOperations();
		void finishAutomaticOperation( size_t id ,bool error , std::string error_message );
		
	private:

		
		void _retreveInfoForTask( ControllerTaskInfo *info );		
		
		virtual void _clear();
		virtual DataManagerCommandResponse _run( size_t task, std::string command );
		virtual void _beginTask( size_t task );
		virtual void _cancelTask( size_t task );
		virtual void _finishTask( size_t task );
		

		
		// Handy functions
		static void copyQueue( Queue * q_from , network::FullQueue * q_to );

		
	};

}

#endif
