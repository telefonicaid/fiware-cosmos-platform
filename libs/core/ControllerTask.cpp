

#include "ControllerTask.h"			// Own interface
#include "JobManager.h"				// ss::JobManager
#include "ControllerDataManager.h"	// ss::ControllerDataManager

namespace ss {

	void ControllerTask::notifyWorkerConfirmation( int worker_id , network::WorkerTaskConfirmation* confirmationMessage ,  ControllerDataManager * data )
	{
		
		if( confirmationMessage->error() )
		{
			error = true;
			error_message = confirmationMessage->error_message();
		}
		

		if ( confirmationMessage->completed() )
		{
			num_completed_workers++;
		}
		
		
		for (int f = 0 ; f < confirmationMessage->file_size() ; f++)
		{
			network::QueueFile qfile = confirmationMessage->file(f);
			network::File file = qfile.file();
			network::KVInfo info = file.info();
			
			std::ostringstream command;
			// add_file worker fileName size kvs queue
			command << "add_file " << file.worker() << " " << file.name() << " " << info.size() << " " << info.kvs() << " " << qfile.queue() << " ";
			
			DataManagerCommandResponse response = data->runOperation( job_id , command.str() );
			
			// Since this is a internally generated command, it can not have any error of format
			assert( !response.error );
		}
	}
	
}

 