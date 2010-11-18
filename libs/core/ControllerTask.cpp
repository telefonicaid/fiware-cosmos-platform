

#include "ControllerTask.h"			// Own interface
#include "JobManager.h"				// ss::JobManager
#include "ControllerDataManager.h"	// ss::ControllerDataManager

namespace ss {

	void ControllerTask::notifyWorkerConfirmation( int worker_id , network::WorkerTaskConfirmation confirmationMessage )
	{
		confirmationMessages.push_back( confirmationMessage );
	}

	void ControllerTask::updateData( ControllerDataManager * data )
	{
		for (int i = 0 ; i < (int) confirmationMessages.size() ; i++)
		{
			for (int f = 0 ; f < confirmationMessages[i].file_size() ; f++)
			{
				network::QueueFile qfile = confirmationMessages[i].file(f);
				network::File file = qfile.file();
				network::KVInfo info = file.info();
				
				std::ostringstream command;
				// add_file worker fileName size kvs queue
				command << "add_file " << file.worker() << " " << file.name() << " " << info.size() << " " << info.kvs() << " " << qfile.queue() << " ";
				
				DataManagerCommandResponse response = data->runOperation( job_id , command.str() );
			
				assert( !response.error );
				// Add this file to this queue
				// XXXX
			}
		}
	}
	
	
}

 