

#include "ControllerDataManager.h"			// Own interface
#include "ControllerTask.h"					// ControllerTask
#include "CommandLine.h"					// au::CommandLine
#include "samson/KVFormat.h"				// ss::KVFormat

namespace ss {

	void ControllerDataManager::updateWithFinishedTask( ControllerTask *task )
	{

		lock.lock();
		
		for (int i = 0 ; i < (int)task->command.size() ; i++)
		{
			au::CommandLine  commandLine;
			commandLine.parse( task->command[i] );
			
			
			if( commandLine.get_num_arguments() == 0)
				continue;
			
			if( commandLine.get_argument(0) == "add_queue" )
			{
				assert( commandLine.get_num_arguments() >= 4);	// This was pre-aproved and sent to the workers so it must confirm the format
				
				std::string name = commandLine.get_argument(1);
				KVFormat format = KVFormat::format( commandLine.get_argument(2) , commandLine.get_argument(3) );

				ControllerQueue *tmp = new ControllerQueue(name , format);
				queue.insert( std::pair< std::string , ControllerQueue*>( name , tmp ) );
			}
			
		}

		lock.unlock();
	}

}