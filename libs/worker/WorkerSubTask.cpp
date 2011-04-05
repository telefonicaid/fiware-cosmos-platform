

#include "WorkerSubTask.h"			// Own interface
#include "WorkerTaskManager.h"		// ss::WorkerTaskManager
#include "WorkerTask.h"				// ss::WorkerTask
#include "ProcessItem.h"			// ss::ProcessItem
#include "ProcessOperation.h"       // ss::ProcessOperation
#include "ProcessCompact.h"         // ss::ProcessCompact
#include "ProcessGenerator.h"		// ss::ProcessGenerator
#include "WorkerTask.h"						// ss::WorkerTask
#include "SamsonSetup.h"					// ss::SamsonSetup
#include "ProcessOperation.h"				// ss::ProcessOperation
#include "ProcessParser.h"					// ss::ProcessParser
#include "DiskOperation.h"					// ss::DiskOperation
#include "SamsonWorker.h"                   // ss::SamsonWorker

namespace ss
{
	
	WorkerSubTask::WorkerSubTask( WorkerTask *_task )
	{
		task = _task;
        
        num_read_operations = 0;
		num_read_operations_confirmed = 0;
        
        num_processes = 0;
        num_processes_confirmed = 0;
        
		description = "U";                  // Unknown   
        status = init;                      // Initial status
        buffer = NULL;                      // By default we have no memory
        
        // Add to receive notification about memory , read operations and process items
        Engine::shared()->notificationSystem.add( notification_memory_request_response , this ); 
        Engine::shared()->notificationSystem.add( notification_disk_operation_request_response , this ); 
        Engine::shared()->notificationSystem.add( notification_process_request_response , this ); 
	}
    
    WorkerSubTask::~WorkerSubTask()
    {
        if( buffer )
        {
            Engine::shared()->memoryManager.destroyBuffer( buffer );
            buffer = NULL;
        }
        
        Engine::shared()->notificationSystem.remove( this ); 
        
    };		
    
    void WorkerSubTask::notify( EngineNotification* notification )
    {
        switch (notification->channel) {
                
            case notification_memory_request_response:
            {
                if( notification->object.size() == 0 )
                    LM_X(1,("Received a notification_memory_request_response without buffer"));
                
                buffer = (Buffer*) notification->object[0];
                run();
                break;
            }
                
            case notification_disk_operation_request_response:
            {
                
                if( notification->object.size() == 0 )
                    LM_X(1,("Received a notification_disk_operation_request_response without DiskOperation object"));
                
                DiskOperation* operation = (DiskOperation*)notification->object[0];
                delete operation;
                
                num_read_operations_confirmed++;
                run();
                break;
            }

            case notification_process_request_response:
            {
                
                if( notification->object.size() == 0 )
                    LM_X(1,("Received a notification_process_request_response without ProcessItem object"));
                
                ProcessItem* item = (ProcessItem*)notification->object[0];
                delete item;
                notification->object.clear();
                
                num_processes_confirmed++;
                run();
                
                break;
            }
                
            default:
                LM_X(1,("WorkerSubTask received an unexpected notification"));
                break;
        }
    }
    
    void WorkerSubTask::run()
    {
        switch (status) {
            case init:
            {
                // Memory request if necessary
                size_t memory = getRequiredMemory();
                status = waiting_memory;
                if( !buffer && (memory > 0) )
                {
                    addMemoryRequest( memory );
                    return;
                }
                else
                    run(); // Recursive call for next step if no buffer is required for this sub task
                break;
            }
                
            case waiting_memory:
            {
                status = waiting_reads;
                run_read_operations();

                if( num_read_operations == 0 )
                    run(); // Recursive calls if no reads are necessary for this task
                
                break;
            }
         
            case waiting_reads:
            {
                if( num_read_operations_confirmed == num_read_operations )
                {

                    status = waiting_process;
                    run_process();
                    
                    if( num_processes == 0 )
                        run();  // Recursive call if no process is required
                }
                break;
            }
                
            case waiting_process:
            {
                if( num_processes_confirmed == num_processes )
                {

                    status = finished;
                    
                    // Notification that this sub-task is finished
                    EngineNotification * notification  = new EngineNotification( notification_sub_task_finished );
                    setNotificationCommandEnvironment(notification);
                    notification->set("target", "WorkerTask");      // Modify to "send" this notification to the worker task
                    Engine::shared()->notify( notification );
                }
                break;
            }
                
            case finished:
            {
                // Nothing to do
            }
                
        }
        
        
        

    }

    bool WorkerSubTask::acceptNotification( EngineNotification *notification )
    {
        //LM_M(("WorkerSubTask: Accept %s" , notification->getDescription().c_str() ));
        
        if( notification->get( "target" , "" ) != "WorkerSubTask" )
            return false;
        
        if( notification->getInt("worker", -1) != task->taskManager->worker->network->getWorkerId() )
            return false;
        
        if( notification->getSizeT( "task_id" , 0 ) != task_id  )
            return false;
        
        if( notification->getSizeT( "sub_task_id" , 0 ) != sub_task_id  )
            return false;

        return true;
    }
    
    void WorkerSubTask::setNotificationCommandEnvironment( EngineNotification *notification)
    {
        notification->set( "target" , "WorkerSubTask" );
        notification->setInt("worker",  task->taskManager->worker->network->getWorkerId() );
        notification->setSizeT( "task_id" , task_id ); 
        notification->setSizeT( "sub_task_id" , sub_task_id ); 
    }
    
    
    void WorkerSubTask::addMemoryRequest( size_t size )
    {
        EngineNotification *memory_request = new EngineNotification( notification_memory_request );
        memory_request->setSizeT( "size", size );
        setNotificationCommandEnvironment(memory_request);
        Engine::shared()->notify( memory_request );
    }
    
    
    void WorkerSubTask::addReadOperation( DiskOperation *operation )
    {
        // Increase the counter of operations to read
        num_read_operations++;
        
        // Add the read operation to the Engine
        EngineNotification *notification = new EngineNotification( notification_disk_operation_request  , operation );
        setNotificationCommandEnvironment(notification);
        Engine::shared()->shared()->notify( notification );
    }
    
    void WorkerSubTask::addProcess( ProcessItem* processItem )
    {
        // Increase the counter of operations to read
        num_processes++;
        
        // Add the read operation to the Engine
        EngineNotification *notification = new EngineNotification( notification_process_request  , processItem );
        setNotificationCommandEnvironment(notification);
        Engine::shared()->shared()->notify( notification );
        
    }
    
    
    std::string WorkerSubTask::getStatus()
    {
        std::ostringstream output;
        output << "[" << description << ":";

        if( error.isActivated() )
            output << "E " << error.getMessage();
        else
        {
            switch (status) {
                case init: output << "I"; break;
                case waiting_memory: output << "M"; break;
                case waiting_reads: output << "R"; break;
                case waiting_process: output << "P"; break;
                case finished: output << "F"; break;
            }
        }
        output << "]";
        return output.str();
    }
    
	
	
#pragma mark GeneratorSubTask
	
	GeneratorSubTask::GeneratorSubTask(WorkerTask * task ) : WorkerSubTask( task  )
	{
		description = "G";
	}
	
	void GeneratorSubTask::run_process()
	{
		addProcess( new ProcessGenerator( task ) );
	}
	
	
#pragma mark OrganizerSubTask
	
	OrganizerSubTask::OrganizerSubTask( WorkerTask *_task  ) : WorkerSubTask( _task )
	{
		
		// Create the reduce information ( stored at the worker task to share with the rest of reduce items )
		task->reduceInformation = new ProcessAssistantSharedFileCollection( task->workerTask );
		
		description = "Or"; // Organizer
		
	}
	
	// Function to get all the read operations necessary for this task
    void OrganizerSubTask::run_read_operations()
	{
		
		for (int f = 0 ; f < task->reduceInformation->total_num_input_files ; f++)
		{
			DiskOperation *item = getFileMangerReadItem( task->reduceInformation->file[f] );
            addReadOperation( item );
		}
	}
	
	// Function to get the ProcessManagerItem to run

	void OrganizerSubTask::run_process()
	{
		ProcessAssistantSharedFileCollection *reduceInformation = task->reduceInformation;
		reduceInformation->setup();
		
		// Organize the reduce in multiple WorkerTaskItems to process each set of hash-groups
		// Division is done in such a way that any of the parts do not get overloaded
		
		// Number of paralel processes in this system
		int num_process = SamsonSetup::shared()->num_processes;
		
		// Maximum size per hash-group
		size_t max_size_per_group = reduceInformation->total_size / num_process;
		
		// Another limit for memory reasons
		size_t max_size_per_group2 = (SamsonSetup::shared()->memory/4) - reduceInformation->total_num_input_files*KVFILE_NUM_HASHGROUPS*sizeof(KVInfo) - sizeof(KVHeader);
		if( max_size_per_group2 < max_size_per_group)
			max_size_per_group = max_size_per_group2;
		
		size_t limit_size_per_group = Engine::shared()->memoryManager.getMemory()/2;
		
		// Create necessary reduce operations
		int hg = 1;												// Evaluating current hash group	
		int	item_hg_begin = 0;									// Starting at hash-group
		size_t total_size = reduceInformation->size_of_hg[0];	// Total size for this operation

		int hg_set = 0 ;                                        // Counter of the hash-group
        
		while( hg < KVFILE_NUM_HASHGROUPS )
		{
			size_t current_hg_size = reduceInformation->size_of_hg[hg];
			
			if( current_hg_size > limit_size_per_group )
			{
				task->setError("Max size for a hash-group exedeed. Operation not suported");
				return;
			}
			
			if( ( ( total_size + current_hg_size  ) > max_size_per_group ) )
			{
				
				if( total_size > 0 )
                {
                    OperationSubTask * tmp = new OperationSubTask( task , item_hg_begin , hg  );
                    tmp->hg_set = hg_set++;
					task->addSubTask( tmp  );
                }
				
				
				// Ready for the next item
				item_hg_begin = hg;
				total_size = current_hg_size;
			}
			else
				total_size+=current_hg_size;
			
			hg++;
		}
		
        OperationSubTask * tmp = new OperationSubTask( task ,item_hg_begin , KVFILE_NUM_HASHGROUPS );
        tmp->hg_set = hg_set++;
		task->addSubTask( tmp );
		
	}
	
	DiskOperation * OrganizerSubTask::getFileMangerReadItem( ProcessAssistantSharedFile* file  )
	{
		// Read the key-value information for each hash group for each input files
		size_t offset			= sizeof( KVHeader );					// We skip the file header
		size_t size				= sizeof(KVInfo) * KVFILE_NUM_HASHGROUPS;
		
		DiskOperation *item = DiskOperation::newReadOperation( file->fileName , offset , size , file->getSimpleBufferForInfo() );
		//item->tag = task->task_id;
		return item;
	}	
	
	
#pragma mark OperationSubTask
	
	OperationSubTask::OperationSubTask( WorkerTask * task , int _hg_begin , int _hg_end  ) : WorkerSubTask( task  )
	{
		// Starting and ending hash-group
		hg_begin = _hg_begin;
		hg_end = _hg_end;
		num_hash_groups = hg_end - hg_begin;
		
		// It will be assigned calling a MemoryRequest
		buffer = NULL;
		
		// Compute the required size for this operation
		memory_requested = task->reduceInformation->total_num_input_files*( ( num_hash_groups )*sizeof(KVInfo) + sizeof(KVHeader) );
		for (int hg = hg_begin ; hg < hg_end ; hg++)
			memory_requested += task->reduceInformation->size_of_hg[hg];
		
		description = "Op";// Operation
	}
	
	OperationSubTask::~OperationSubTask()
	{
	}
	
	size_t OperationSubTask::getRequiredMemory()
	{
		return memory_requested;
	}
	
	void OperationSubTask::run_read_operations()
	{
        // If no memory, means that no process is required
		if( memory_requested == 0)
            return;
        
		// Pointer to the buffer
		char* data = buffer->getData();
		
		// Reduce information is stored in the parent task ( common to all reduce task-items )
		ProcessAssistantSharedFileCollection *reduceInformation = task->reduceInformation;
		
		// Offset while writing into the shared memory area
		size_t offset = 0;		
		
		// Write all files at the shared memory
		for (int f = 0 ;  f < reduceInformation->total_num_input_files ; f++)
		{
			// Header
			KVHeader header = reduceInformation->file[f]->getKVHeader( hg_begin , hg_end );
			memcpy(data+offset, &header, sizeof(KVHeader));
			offset+= sizeof(KVHeader);
			
			// Copy the info vector
			size_t size_info = ( hg_end - hg_begin ) * sizeof( KVInfo );
			memcpy(data + offset , &task->reduceInformation->file[f]->info[hg_begin],  size_info  );
			offset += size_info;
			
			// Schedule the read operation into the FileManager to read data content
			DiskOperation *item 
			= DiskOperation::newReadOperation( \
									  reduceInformation->file[f]->fileName , \
									  reduceInformation->file[f]->getFileOffset( hg_begin ), \
									  header.info.size, \
									  buffer->getSimpleBufferAtOffset(offset) );
			
			addReadOperation( item );
			
			offset += header.info.size;
		}
		
		
	}
	
	// Function to get the ProcessManagerItem to run
	void OperationSubTask::run_process()
	{
        // If no memory, means that no process is required
		if( memory_requested == 0)
            return;
        
		ProcessOperation * item = new ProcessOperation( this );
        item->hg_set = hg_set;
        addProcess(item);
	}
	
	
#pragma mark  ParserSubTask
	
	
	ParserSubTask::ParserSubTask( WorkerTask * task ,  std::string _fileName  ) : WorkerSubTask( task  )
	{
		// Filename to parse
		fileName = _fileName;
		
		// It will be assigned calling a MemoryRequest
		buffer = NULL;
		
		// Compute the required size for this operation
		fileSize = au::Format::sizeOfFile( SamsonSetup::shared()->dataDirectory + "/" + fileName );	
		
		description = "P"; // Parser
	}
	
	ParserSubTask::~ParserSubTask()
	{
	}
	
	size_t ParserSubTask::getRequiredMemory()
	{
		return fileSize;
	}
	
	
	void ParserSubTask::run_read_operations()
	{
        
        if( fileSize > 0)
        {
            if( !buffer )
                LM_X(1,("Intern error: No buffer in read operations of task"));
            // Single file to be parsed
            DiskOperation *item = DiskOperation::newReadOperation( fileName , 0, fileSize, buffer->getSimpleBuffer() );
            addReadOperation(item);
        }
		
	}
	
	// Function to get the ProcessManagerItem to run
	void ParserSubTask::run_process()
	{
		addProcess(new ProcessParser( this ) );
	}
	
#pragma mark SystemSubTask
	
	SystemSubTask::SystemSubTask( WorkerTask *_task  ) : WorkerSubTask( _task )
	{
		// Create the reduce information ( stored at the worker task to share with the rest of reduce items )
		task->reduceInformation = new ProcessAssistantSharedFileCollection( task->workerTask );
		description = "Sys"; // System
		
		// add all the File elements to be removed when the operation is complete
		for (int i = 0 ; i < task->workerTask.input_queue(0).file_size() ; i++)
		{
            /*
			network::QueueFile qf;
			qf.set_queue( task->workerTask.input_queue(0).queue().name() );
			qf.mutable_file()->CopyFrom( task->workerTask.input_queue(0).file(i) );
			task->removeFile( qf );
             */
            
            LM_TODO(("Reimplement using notifications!!"));
		}
	}
	
	// Function to get all the read operations necessary for this task
	void SystemSubTask::run_read_operations()
	{
		for (int f = 0 ; f < task->reduceInformation->total_num_input_files ; f++)
		{
			DiskOperation *item = getFileMangerReadItem( task->reduceInformation->file[f] );
            addReadOperation(item);
		}
	}
	
	// Function to get the ProcessManagerItem to run
	
	void SystemSubTask::run_process()
	{
		
		ProcessAssistantSharedFileCollection *reduceInformation = task->reduceInformation;
		reduceInformation->setup();
		
		// Organize the content so, that we generate necessary files
		size_t max_item_content_size = SamsonSetup::shared()->max_file_size;
		
		// Create necessary reduce operations
		int hg = 1;												// Evaluating current hash group	
		int	item_hg_begin = 0;									// Starting at hash-group
		size_t total_size = reduceInformation->size_of_hg[0];	// Total size for this operation
		
		while( hg < KVFILE_NUM_HASHGROUPS )
		{
			size_t current_hg_size = reduceInformation->size_of_hg[hg];
			
			if( ( ( total_size + current_hg_size  ) > max_item_content_size ) )
			{
				
				if( total_size > 0 )
					task->addSubTask( new CompactSubTask( task , item_hg_begin , hg  ) );
				
				
				// Ready for the next item
				item_hg_begin = hg;
				total_size = current_hg_size;
			}
			else
				total_size+=current_hg_size;
			
			hg++;
		}
		
		task->addSubTask( new CompactSubTask( task ,item_hg_begin , KVFILE_NUM_HASHGROUPS ) );
		
		// No real process for this sub-task
	}
	
	DiskOperation * SystemSubTask::getFileMangerReadItem( ProcessAssistantSharedFile* file  )
	{
		// Read the key-value information for each hash group for each input files
		size_t offset			= sizeof( KVHeader );					// We skip the file header
		size_t size				= sizeof(KVInfo) * KVFILE_NUM_HASHGROUPS;
		
		DiskOperation *item = DiskOperation::newReadOperation( file->fileName , offset , size , file->getSimpleBufferForInfo() );
		//item->tag = task->task_id;
		return item;
	}		
	
#pragma mark -
	
	
	CompactSubTask::CompactSubTask( WorkerTask * task , int _hg_begin , int _hg_end  ) : WorkerSubTask( task  )
	{
		// Starting and ending hash-group
		hg_begin = _hg_begin;
		hg_end = _hg_end;
		num_hash_groups = hg_end - hg_begin;
		
		// It will be assigned calling a MemoryRequest
		buffer = NULL;
		
		// Compute the required size for this operation
		memory_requested = task->reduceInformation->total_num_input_files * ( sizeof(KVHeader) + ( num_hash_groups )*sizeof(KVInfo) );
		for (int hg = hg_begin ; hg < hg_end ; hg++)
			memory_requested += task->reduceInformation->size_of_hg[hg];
		
		description = "Sys";// System
	}
	
	CompactSubTask::~CompactSubTask()
	{
	}
	
	
	
	size_t CompactSubTask::getRequiredMemory()
	{
		return memory_requested;
	}
	
	
	void CompactSubTask::run_read_operations()
	{
		
		// Pointer to the buffer
		char* data = buffer->getData();
		
		// Reduce information is stored in the parent task ( common to all reduce task-items )
		ProcessAssistantSharedFileCollection *reduceInformation = task->reduceInformation;
		
		// Offset while writing into the shared memory area
		size_t offset = 0;		
		
		// Write all files at the shared memory
		for (int f = 0 ;  f < reduceInformation->total_num_input_files ; f++)
		{
			// Header
			KVHeader header = reduceInformation->file[f]->getKVHeader( hg_begin , hg_end );
			memcpy(data+offset, &header, sizeof(KVHeader));
			offset+= sizeof(KVHeader);
			
			// Copy the info vector
			size_t size_info = ( hg_end - hg_begin ) * sizeof( KVInfo );
			memcpy(data + offset , &task->reduceInformation->file[f]->info[hg_begin],  size_info  );
			offset += size_info;
			
			// Schedule the read operation into the FileManager to read data content
			DiskOperation *item 
			= DiskOperation::newReadOperation( \
									  reduceInformation->file[f]->fileName , \
									  reduceInformation->file[f]->getFileOffset( hg_begin ), \
									  header.info.size, \
									  buffer->getSimpleBufferAtOffset(offset) );
			
            // Add this item to be read
            addReadOperation(item);
            
			offset += header.info.size;
		}
		
	}
	
	// Function to get the ProcessManagerItem to run
	void CompactSubTask::run_process()
	{
		addProcess( new ProcessCompact( this ) );
	}
	
	
}
