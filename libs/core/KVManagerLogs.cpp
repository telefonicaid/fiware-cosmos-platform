#include <string>

#include "samsonLogMsg.h"
#include "KVManagerLogs.h"
#include "samson.h"
#include "KVManager.h"
#include "Task.h"
#include "MemoryController.h"
#include "KVQueue.h"
#include "KVSet.h"
#include "KVSetsManager.h"
#include "KVFormat.h"
#include "Data.h"
#include "Module.h"
#include "KVFormat.h"
#include "SSLogger.h"
#include "SSMonitor.h"
#include "KVSetBuffer.h"
#include "TaskManager.h"
#include "Tasks.h"



namespace ss
{
#pragma mark Log
	
	
	void setFormat( protocol::Format *format, KVFormat _format )
	{
		format->set_key( _format.keyFormat );
		format->set_value( _format.valueFormat );
	}
	
	void KVManagerLog::addQueue( std::string queue_name , KVFormat format )
	{
		protocol::Queue *queue = add_add_queue();
		queue->set_name( queue_name );
		setFormat( queue->mutable_format() , format );
		
	}
	
	void KVManagerLog::addKVSetVectorToKVQueue( KVSetVector* sets , std::string queue_name , int hash )
	{
		protocol::SetVector *_sets = add_add_sets();
		
		_sets->set_queue_name( queue_name );
		_sets->set_hash( hash );
		
		for (size_t s = 0 ; s < sets->size() ; s++)
		{
			protocol::Set *set = _sets->add_set();
			
			set->set_id( (*sets)[s]->id );
			set->set_task_id( (*sets)[s]->task_id );
			set->set_size( (*sets)[s]->getSize() );
			set->set_num_kvs( (*sets)[s]->getNumKVs() );
			setFormat( set->mutable_format() , (*sets)[s]->getFormat() );
		}
		
	}
	
	void KVManagerLog::addKVSetToKVQueue( KVSet* set , std::string queue_name , int hash )
	{
		protocol::SetVector *_sets = add_add_sets();
		
		_sets->set_queue_name( queue_name );
		_sets->set_hash( hash );
		
		protocol::Set *_set = _sets->add_set();
		
		_set->set_id( set->id );
		_set->set_task_id( set->task_id );
		_set->set_size( set->getSize() );
		_set->set_num_kvs( set->getNumKVs() );
		setFormat( _set->mutable_format() , set->getFormat() );
		
	}
	
	
	void KVManagerLog::removeQueue( std::string queue_name )
	{
		protocol::Queue *queue = add_remove_queue();
		queue->set_name( queue_name );
		
	}
	
	void KVManagerLog::removeKVSetVectorToKVQueue( KVSetVector* sets , std::string queue_name , int hash )
	{
		protocol::SetVector *_sets = add_remove_sets();
		
		_sets->set_queue_name( queue_name );
		_sets->set_hash( hash );
		
		for (size_t s = 0 ; s < sets->size() ; s++)
		{
			protocol::Set *set = _sets->add_set();
			
			set->set_id( (*sets)[s]->id );
			set->set_task_id( (*sets)[s]->task_id );
			set->set_size( (*sets)[s]->getSize() );
			set->set_num_kvs( (*sets)[s]->getNumKVs() );
			setFormat( set->mutable_format() , (*sets)[s]->getFormat() );
		}
		
	}	
	
	
	void KVManagerLog::addQueue( KVQueue *_queue )
	{
		// add the queue itself
		addQueue( _queue->name , _queue->format );
		
		// Add all the sets included in each subQueue
		
		for (int h = 0 ; h < NUM_SETS_PER_STORAGE ; h++)
		{
			KVSetVector *sets = _queue->getKVSets(h, false);
			addKVSetVectorToKVQueue(sets, _queue->name, h);
			delete sets;
		}
	}
	
	
	// New KVSet
	void KVManagerLog::addKVSet( size_t set_id )
	{
		add_newset( set_id );
	}
	
	// Notification that set has been saved to disk
	void KVManagerLog::addSavedToDisk( size_t set_id )
	{
		add_savedset( set_id );
	}
	
#pragma mark KVManagerLogBuffer
	
	void KVManagerLogBuffer::process( KVManagerLog *log )
	{
		
		logs.push_back( log );
		
		// Add new sets?
		for (int j = 0 ; j < log->newset_size() ; j++)
			counter_sets++;			
		
		// Flush to disk operations ( not related with real task id )
		for (int j = 0 ; j < log->savedset_size() ; j++)
			counter_sets--;			
		
		if( ( log->task() == top_level_task) && ( log->task_code() == TASK_CODE_END ) )
		{
			finish = true;
		}
		
		
		if( finish && (counter_sets ==0) )
		{
			validated =true;
		}
		
		
	}
	
	void KVManagerLogBuffer::runAll( KVManager *manager )
	{
		for (size_t i = 0 ; i < logs.size() ; i++)
			manager->runOperation(logs[i]);
	}
	
	
#pragma mark KVManagerLogProcessor
	
	KVManagerLogProcessor::KVManagerLogProcessor( KVManager *_manager )
	{
		manager = _manager;
	}
	
	
	size_t KVManagerLogProcessor::getParentTask( size_t task_id )
	{
		if( task_id == 0)
			return 0;
		
		std::map<size_t , size_t>::iterator i = parents.find( task_id );
		assert( i != parents.end() );
		return i->second;
	}
	
	size_t KVManagerLogProcessor::getTopTask( size_t task_id )
	{
		size_t parent_task_id = getParentTask( task_id  );
		
		while( parent_task_id != 0)
		{
			task_id = parent_task_id;
			parent_task_id = getParentTask( task_id );
		}
		
		return task_id;
		
	}
	
	void KVManagerLogProcessor::setParentTask( size_t task_id , size_t parent_task_id)
	{
		parents.insert ( std::pair<size_t , size_t>( task_id , parent_task_id ) );
	}
	
	
	
	void KVManagerLogProcessor::run( std::string fileName )
	{
		
		// Read the log file to recover previous state
		char buffer[ 10000 ];
		FILE *log = fopen( fileName.c_str() , "r" );
		if ( log )
		{
			while( !feof(log) )
			{
				
				size_t size;
				size_t r;
				
				r = fread(&size, 1, sizeof(size) , log );	// Read the size
				if ( r > 0 )
				{
					// Read the operation
					r = fread(&buffer , 1 , size , log);		// Read the operation itself
					assert( r == size);
					
					// Parse the message
					KVManagerLog *operation = new KVManagerLog();
					operation->ParseFromArray(buffer, size);
					
					
					// Insert information about parent tasks
					if( operation->has_parenttask() )
						setParentTask( operation->task() , operation->parenttask() );
					
					// Get all information about this log entry
					size_t task_id = operation->task();
					size_t top_task_id = getTopTask(task_id);
					
					
					if( task_id == 0)
					{
						// Direct operation if task_id = 0 "system level operation"
						manager->runOperation( operation );
						delete operation;
					}
					else
					{
						
						// New top level operation
						if( ( operation->task_code() == TASK_CODE_BEGIN ) && ( operation->parenttask() == 0) )
						{
							KVManagerLogBuffer *tmp  = new KVManagerLogBuffer(task_id);
							logs.insert( std::pair< size_t , KVManagerLogBuffer *>( task_id , tmp) );
						}
						
						// Find the logBuffer
						KVManagerLogBuffer *logBuffer = NULL;
						std::map< size_t , KVManagerLogBuffer *>::iterator iterator_logbuffer = logs.find( top_task_id );
						if( iterator_logbuffer!= logs.end() )
							logBuffer = iterator_logbuffer->second;
						assert( logBuffer );
						
						
						logBuffer->process(  operation );
						
						if( logBuffer->validated )
						{
							logBuffer->runAll( manager );
							delete logBuffer;
							logs.erase( iterator_logbuffer );
						}
					}
					
					
				}
			}
			fclose(log);
		}
	}
	
	
	
	
	
	
#pragma mark KVManagerLogs
	
	void KVManagerLogs::setupFromLog( KVManager *manager )
	{
		KVManagerLogProcessor p( manager );
		p.run( _logFileName() );
	}
	
	std::string KVManagerLogs::_logFileName( )
	{
		std::ostringstream o;
		o << SAMSON_WORKSPACE_DIRECTORY << "/log";
		return o.str();
	}
	
	void KVManagerLogs::_logOperation( KVManagerLog *operation )
	{
		_logOperation(operation, log);
	}
	
	void KVManagerLogs::_logOperation( KVManagerLog* operation , FILE *file )
	{
		char buffer[ 100000 ];
		size_t size = operation->ByteSize();

		if (size >= 100000)
		{
			LOG_ERROR(("size too big: %llu", (long long unsigned int) size));
			assert( size < 100000 );
		}

		bool ans = operation->SerializeToArray(buffer, size);
		if (!ans)
		{
			LOG_ERROR(("SerializeToArray failed"));
			assert( ans );
		}
		
		// Write on disk
		size_t size_written;

		size_written = fwrite(&size, 1, sizeof(size), file);
		if (size_written != sizeof(size))
		{
			LOG_ERROR(("written %llu bytes (%llu wanted)", (long long unsigned int) size_written, (long long unsigned int) sizeof(size)));
			assert(0);
		}

		size_written = fwrite(buffer, 1, size, file);
		if (size_written != size)
		{
            LOG_ERROR(("written %llu bytes (%llu wanted)", (long long unsigned int) size_written, (long long unsigned int) size));
            assert(0);
		}

		fflush(file);
	}
	
	void KVManagerLogs::_restoreWithUniqueLog( KVManagerLog *log )
	{
		std::string newLogName = _logFileName() + ".tmp";
		FILE *new_log = fopen( newLogName.c_str() , "w" );
		_logOperation( log , new_log );
		fclose( new_log );
		rename( newLogName.c_str() , _logFileName().c_str() );
	}
	
	void KVManagerLogs::_startLog()
	{
		log = fopen( _logFileName().c_str() , "a" );
		if( !log )
		{
			std::cout << "Error, could not open log file for platform activity\n";
			exit(0);
		}
	}
	void KVManagerLogs::_stopLog()
	{
		fclose(log);
	}	
}
