

#include "ProcessCompact.h"     // Own interface
#include "samson/module/ModulesManager.h"     // samson::ModulesManager
#include "WorkerTask.h"         // samson::WorkerTask
#include "WorkerSubTask.h"      // samson::OrganizerSubTask
#include "samson/network/Packet.h"             // samson::Packet
#include <algorithm>            // std::sort
#include "WorkerTaskManager.h"  // samson::TaskManager
#include "samson/worker/SamsonWorker.h"       // samson::SamsonWorker

#include "samson/common/MemoryTags.h"         // MemoryOutputDisk

namespace samson
{

#pragma mark -
  
  ProcessCompact::ProcessCompact( CompactSubTask * _compactSubTask ) : ProcessItem( PI_PRIORITY_NORMAL_COMPACT )
  {
    compactSubTask = _compactSubTask;

    // Informationk about the generated file
    fileName = compactSubTask->task->newFileName(); 
    queue = compactSubTask->task->workerTask.input_queue(0).queue().name();  

    // Pointer to the task manager to report when finish
    tm = compactSubTask->task->taskManager;
    task_id = compactSubTask->task->task_id;

  }
  
  ProcessCompact::~ProcessCompact()
  {
  }

  
  void ProcessCompact::run()
  {
    char *data = compactSubTask->buffer->getData();
    
    int num_inputs=  compactSubTask->task->workerTask.input_queue_size();
    int num_input_files = 0;
    for (int i = 0 ; i < num_inputs ; i++)
      num_input_files += compactSubTask->task->workerTask.input_queue(i).file_size();
    
    // If no input files, no operation is needed
    if( num_input_files == 0)
      return;
    
    // Process all input files [ Header ] [ Info ] [ Data ]
    ProcessSharedFile *reduce_file = new ProcessSharedFile[ num_input_files ];
    size_t offset = 0;
    for (int i = 0 ; i < num_input_files ; i++ )
      offset += reduce_file[i].set( data + offset );
    
    // Get the number of hash groups and make sure all the files have the same number
    uint32 num_hash_groups = reduce_file[0].header->getNumHashGroups();

    // Rigth now, compact only work with full-hash-group files
    if( num_hash_groups != KVFILE_NUM_HASHGROUPS)
		LM_X(1,("Internal error: Compact operation can only process full-hash-group files"));
	  
	  // Compact operation only works with 1 input
	  if( num_inputs != 1 )
		  LM_X(1,("Internal error: Compact operation can only process one input"));
	  
	  KVInfo total_info;
	  total_info.clear();
	  size_t outputBufferSize = sizeof( KVHeader ) + sizeof(KVInfo)*KVFILE_NUM_HASHGROUPS; 
	  for (uint32 i = 0 ; i < (uint32)num_input_files ; i++ )
      {
		  if( reduce_file[i].header->getNumHashGroups() != num_hash_groups)
			  LM_X(1,("Internal error: Invalid number of hash-groups in compact operation" ));
		  total_info.append( reduce_file[i].header->info );
		  
		  outputBufferSize += reduce_file[i].header->info.size;// Total size of data buffer
      }
	  
	  // create the output buffer with the rigth size
	  engine::Buffer *outputBuffer = engine::MemoryManager::shared()->newBuffer( "Compact output buffer", outputBufferSize , MemoryOutputDisk );
	  
	  // Output header and KVInfo vector
	  KVHeader *outputHeader = (KVHeader*) outputBuffer->getData();
      KVInfo *outputInfo = (KVInfo*) ( outputBuffer->getData() + sizeof(KVHeader) );
      char *outputData = outputBuffer->getData() + sizeof( KVHeader ) + sizeof(KVInfo) * num_hash_groups;
    
    // Contents of the header
    outputHeader->init( reduce_file[0].header->getFormat() , total_info );
    outputHeader->setHashGroups( 0 , KVFILE_NUM_HASHGROUPS );// Full hash-group
    
    for (uint32 hg = 0 ; hg < num_hash_groups ; hg++)
      {
	outputInfo[hg].clear();
	
	// Counte the number of key-values I will have in this round
	for (int f = 0 ; f < num_input_files ; f++)
	  {
	    outputInfo[hg].append( reduce_file[f].info[hg] );
	    
	    if ( reduce_file[f].info[hg].size > 0 )
	      {
		memcpy(outputData, reduce_file[f].hg_data[hg], reduce_file[f].info[hg].size );
		outputData += reduce_file[f].info[hg].size;
	      }
	    
	  }
      }
    
    // Set the maximum size ( it was allocated using the final size )
    outputBuffer->setSize( outputBuffer->getMaxSize() );
    
    // Queue File to report new file
    network::QueueFile qf;
    qf.set_queue( queue );
    
    network::File *file = qf.mutable_file();
    file->set_name( fileName );
    file->set_worker( tm->worker->network->getWorkerId() );
    network::KVInfo *info = file->mutable_info();
    
    info->set_size( total_info.size );
    info->set_kvs( total_info.kvs );
    
    // Create the new files using the new buffer
    //tm->addFile( task_id , qf , outputBuffer );
      
      LM_TODO(("Reimplement using notifications!!"));

  }



}
