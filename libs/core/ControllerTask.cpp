

#include "ControllerTask.h"			// Own interface
#include "JobManager.h"				// ss::JobManager
#include "ControllerDataManager.h"	// ss::ControllerDataManager

namespace ss {

	ControllerTask::ControllerTask( size_t _id , Job *_job, ControllerTaskInfo *_info , int _total_workers )
	{
		// Keep the command and the id
		id = _id;
		
		// Keep a pointer to the job
		job = _job;
		
		// Elements
		info = _info;
		
		error = false;
		
		// total number of workers to wait for this number of confirmation ( in case we sent to workers )
		num_workers = _total_workers;
		
		finish = false;
		complete = false;
		
		
		// In generators, this is used to determine how is the active user
		generator = rand()%num_workers;
		
		num_items = (int *) malloc( sizeof(int) * num_workers );
		num_finish_items = (int *) malloc( sizeof(int) * num_workers );
		
		for (int i = 0 ; i < num_workers ; i++)
		{
			num_items[i] = 0;
			num_finish_items[i] = 0;
		}
		total_num_finish_items = 0;
		total_num_items = 0 ;
		
	}
	
	ControllerTask::~ControllerTask()
	{
		delete info;
		
		free( num_items );
		free( num_finish_items );
	}
	
	size_t ControllerTask::getId()
	{
		return id;
	}
	
	size_t ControllerTask::getJobId()
	{
		return job->id;
	}	
	
	void ControllerTask::notifyWorkerConfirmation( int worker_id , network::WorkerTaskConfirmation* confirmationMessage ,  ControllerDataManager * data )
	{
		if( confirmationMessage->error() )
		{
			error = true;
			error_message = confirmationMessage->error_message();
		}

		for (int f = 0 ; f < confirmationMessage->file_size() ; f++)
		{
			network::QueueFile qfile = confirmationMessage->file(f);
			network::File file = qfile.file();
			network::KVInfo info = file.info();
			
			std::ostringstream command;
			// add_file worker fileName size kvs queue
			command << "add_file " << file.worker() << " " << file.name() << " " << info.size() << " " << info.kvs() << " " << qfile.queue() << " ";
			
			DataManagerCommandResponse response = data->runOperation( job->id , command.str() );
			
			// Since this is a internally generated command, it can not have any error of format
			assert( !response.error );
		}
		
		// Update the status of this task in terms of worker items
		num_items[worker_id]		= confirmationMessage->num_items();
		num_finish_items[worker_id] = confirmationMessage->num_finish_items();

		// Update information in the job
		int total = 0 ;
		int num = 0 ;
		for (int i = 0 ; i < num_workers ; i++)
		{
			total += num_items[i];
			num += num_finish_items[i];
		}
		total_num_finish_items = num;
		total_num_items = total;
		
		
		
		if ( confirmationMessage->finish() )
			finish_workers.insert( worker_id );

		if ( confirmationMessage->completed() )
			complete_workers.insert( worker_id );
		
		
		if( (int)finish_workers.size() == num_workers )
			finish = true;

		if( (int)complete_workers.size() == num_workers )
			complete = true;
	}

	
	void ControllerTask::fillInfo( network::WorkerTask *t , int workerIdentifier )
	{
		t->set_operation( info->operation_name );
		
		// Set input files
		for (size_t f = 0 ; f < info->input_files.size() ; f++)
		{
			network::FileList all_fl = info->input_files[f];
			network::FileList *fl = t->add_input();
			
			// Add only files that are placed at that worker
			for (int i = 0 ; i < all_fl.file_size() ; i++)
			{
				if( all_fl.file(i).worker() == workerIdentifier)
					fl->add_file()->CopyFrom( all_fl.file(i) );
			}
			
			//fl->CopyFrom();
		}
		
		
		// Set the output queues
		for (int i = 0 ; i < (int)info->outputs.size() ; i++)
		{
			network::Queue *q = t->add_output();
			network::Queue qq = info->output_queues[i]; 
			q->CopyFrom( qq );
		}
		
		
		// Set environment variables
		Environment *env = &info->job->environment;
		
		network::Environment *e = t->mutable_environment();	
		std::map<std::string,std::string>::iterator iter;
		for ( iter = env->environment.begin() ; iter != env->environment.end() ; iter++)
		{
			network::EnvironmentVariable *ev = e->add_variable();			
			
			ev->set_name( iter->first );
			ev->set_value( iter->second );
		}
		
	}
	
	
}

 