


#include "engine/ProcessItem.h"				// engine::ProcessItem
#include "engine/Engine.h"					// engine::Engine
#include "engine/ProcessManager.h"          // engine::ProcessManager

#include "ProcessIsolated.h"                // Own interface
#include "ProcessWriter.h"
#include "SharedMemoryManager.h"


#include "samson/network/NetworkInterface.h"
#include "samson/network/Packet.h"

namespace samson
{
    ProcessIsolated::ProcessIsolated( std::string description, ProcessBaseType _type ) : ProcessItemIsolated( description )
    {
        num_outputs = 0;        // Outputs are defined calling "addOutput" with the rigth output format
        
        type = _type;
        
        // By default we have no asignation of shared memory
        shm_id = -1;
        item = NULL;
        
        writer = NULL;
        txtWriter = NULL;
    }
    
    ProcessIsolated::~ProcessIsolated()
    {
        //LM_M(("Destroying ProcessIsolated"));
        
        if( shm_id != -1 )
            engine::SharedMemoryManager::shared()->releaseSharedMemoryArea( shm_id );
                
        if( writer )
            delete writer;
        
        if( txtWriter )
            delete txtWriter;
    }
    
    // Function to specify if we are ready to be executed of continued from a halt
    bool ProcessIsolated::isReady()
    {
        if( shm_id == -1 )
        {
            // Try to get a shared memory buffer to produce output
            shm_id = engine::SharedMemoryManager::shared()->retainSharedMemoryArea();
            if( shm_id != -1 )
                item = engine::SharedMemoryManager::shared()->getSharedMemoryPlatform( shm_id );
        }
        
        bool available_memory = true;
        
        engine::MemoryManager *mm = engine::MemoryManager::shared();
        double memory_output_network    = mm->getMemoryUsageByTag( MemoryOutputNetwork );
        double memory_output_disk       = mm->getMemoryUsageByTag( MemoryOutputDisk );
        
        if( (memory_output_network + memory_output_disk ) > 0.5 )
            available_memory = false;

        if( !available_memory )
        {
            //LM_M(("ProcessItem not ready since there is not output memory "));
            return false;
        }
        
        if ( !item )
        {
            //LM_M(("ProcessItem not ready since there is not shared memory"));
            return false;
        }

        return true;
    }        
    
    // Get the writers to emit key-values
    ProcessWriter* ProcessIsolated::getWriter()
    {
        if( type != key_value )
            LM_X(1, ("Internal error: Not possible to get a writer if type!=key_value"));
        
        if( ! writer )
            writer = new ProcessWriter( this );
        return writer;
    }
    
    ProcessTXTWriter* ProcessIsolated::getTXTWriter()
    {
        if( type != txt )
            LM_X(1, ("Internal error: Not possible to get a txtWriter if type!=txt"));
        
        if( ! txtWriter )
            txtWriter = new ProcessTXTWriter( this );
        return txtWriter;
    }  

    void ProcessIsolated::runCode( int c )
    {
        //LM_M(("Isolated process Running code %d",c));
        
        switch (c) {
            case WORKER_TASK_ITEM_CODE_FLUSH_BUFFER:
                flushBuffer(false);	// Flush the generated buffer with new key-values
                return;
                break;
            case WORKER_TASK_ITEM_CODE_FLUSH_BUFFER_FINISH:
                flushBuffer(true);	// Flush the generated buffer with new key-values
                return;
                break;
            default:
                error.set("System error: Unknown code in the isolated process communication");
                break;
        }
        
        //LM_M(("Finish Isolated process Running code %d",c));
    }
    
    
	void ProcessIsolated::flushBuffer( bool finish )
	{
		switch (type) {
			case key_value:
				flushKVBuffer(finish);
				break;
			case txt:
				flushTXTBuffer(finish);
				break;
		}
        		
	}
    
	void ProcessIsolated::flushKVBuffer( bool finish )
	{
		
		/*
		 After flushing we check that available memory is under 100%.
		 Otherwise we halt notifying this to the ProcessManager
		 */

        while( !isReady() )
			halt();
		
#pragma mark ---		
		
		// General output buffer
		char * buffer = item->data;
		size_t size = item->size;
		
		// Make sure everything is correct
		if( !buffer )
			LM_X(1,("Internal error: Missing buffer in ProcessBase"));
		if( size == 0)
			LM_X(1,("Internal error: Wrong size for ProcessBase"));
        
		
		// Outputs structures placed at the begining of the buffer
		OutputChannel *channel = (OutputChannel*) buffer;
		
		// Buffer starts next
        size_t num_workers = distribution_information.workers.size();
		NodeBuffer* node = (NodeBuffer*) ( buffer + sizeof(OutputChannel) * num_outputs * num_workers );
		//size_t num_nodes = ( size - (sizeof(OutputChannel)* num_outputs* num_servers )) / sizeof( NodeBuffer );
		
#pragma mark ---
		
		//size_t task_id = task->workerTask.task_id();
		
		for (int o = 0 ; o < num_outputs ; o++)
		{
			
			for (size_t s = 0 ; s < num_workers ; s++)
			{				
                
				OutputChannel * _channel = &channel[ o * num_workers + s ];	
				
				if( _channel->info.size > 0)
				{
                    
                    engine::Buffer *buffer = engine::MemoryManager::shared()->newBuffer( "ProcessWriter", KVFILE_TOTAL_HEADER_SIZE + _channel->info.size , MemoryOutputNetwork );
					if( !buffer )
						LM_X(1,("Internal error: Missing buffer in ProcessBase"));
					
					// Pointer to the header
					KVHeader *header = (KVHeader*) buffer->getData();
					
					// Pointer to the info vector
					KVInfo *info = (KVInfo*) (buffer->getData() + sizeof( KVHeader ));
					
					// Initial offset for the buffer to write data
					buffer->skipWrite(KVFILE_TOTAL_HEADER_SIZE);
                    
					//KVFormat format = KVFormat( output_queue.format().keyformat() , output_queue.format().valueformat() );
                    if( outputFormats.size() > (size_t)o )
                        header->init( outputFormats[o] , _channel->info );
                    else
                        header->init( KVFormat( "no-used" , "no-used" ) , _channel->info );
					
					for (int i = 0 ; i < KVFILE_NUM_HASHGROUPS ; i++)
					{
                        
						HashGroupOutput * _hgOutput	= &_channel->hg[i];							// Current hash-group output
						
						// Set gloal info
						info[i] = _hgOutput->info;
						
						// Write data followign nodes
						uint32 node_id = _hgOutput->first_node;
						
						while( node_id != KV_NODE_UNASIGNED )
						{
							bool ans = buffer->write( (char*) node[node_id].data, node[node_id].size );
							if( !ans )
								LM_X(1,("Error writing key-values into a temporal Buffer ( size %lu ) " , node[node_id].size ));
							
							// Go to the next node
							node_id = node[node_id].next;
						}
					}
                    					
					if( buffer->getSize() != buffer->getMaxSize() )
						LM_X(1,("Internal error"));

                    
                    // Set the hash-group limits of the header
                    header->range.setFrom( info );
                    //header->range = KVRange( 0 , KVFILE_NUM_HASHGROUPS );

                    //LM_M(("Output buffer for operation %s with range %s", operation_name.c_str() , header->range.str().c_str() ));
                    
                    
                    // Process the output buffer
                    processOutputBuffer(buffer, o, s, finish);
                    
					
				}
			}
			
		}
		
	}	
    
	
	void ProcessIsolated::flushTXTBuffer( bool finish )
	{
        
		/*
		 After flushing we check that available memory is under 100%.
		 Otherwise we halt notifying this to the ProcessManager
		 */
		
        while( !isReady() )
			halt();
		
#pragma mark ---		
		
		
		// Size if the firt thing in the buffer
		size_t size = *( (size_t*) item->data );
		
		// Init the data buffer used here
		char *data = item->data + sizeof(size_t);
		
#pragma mark ---		
		
		if( size > 0 )
		{
			
			//size_t task_id = task->workerTask.task_id();
			
			engine::Buffer *buffer = engine::MemoryManager::shared()->newBuffer( "ProcessTXTWriter", sizeof(KVHeader) + size , MemoryOutputNetwork );

			if( !buffer )
				LM_X(1,("Internal error"));
			
            KVHeader *header = (KVHeader *) buffer->getData();
            header->initForTxt( size );
            
			// copy the entire buffer to here
			memcpy(buffer->getData() + sizeof( KVHeader ) , data, size);
			buffer->setSize( sizeof(KVHeader) + size );
            
            processOutputTXTBuffer(buffer, finish);
		}
		
	}	    
    
    void ProcessIsolated::runIsolated()
    {
        
        switch (type) {
                
            case key_value:
                generateKeyValues( getWriter() );
                getWriter()->flushBuffer(true);
                break;
                
            case txt:
                // Generate TXT content using the entire buffer
                generateTXT( getTXTWriter() );
                getTXTWriter()->flushBuffer(true);
                break;
        }
        
    }
    
    void ProcessIsolated::addOutput( KVFormat format )
    {
        num_outputs++;
        outputFormats.push_back( format );
    }
    
    void ProcessIsolated::addOutputsForOperation( Operation *op )
    {
        for (int i = 0 ; i < op->getNumOutputs() ; i++)
            addOutput( op->getOutputFormat(i) );
        
        // Add an additional output for log
        addOutput( KVFormat("system.String", "system.Void") );
    }
   
    void ProcessIsolated::setDistributionInformation( DistributionInformation _distribution_information  )
    {
        distribution_information = _distribution_information;
    }
    
    void ProcessIsolated::setProcessBaseMode(ProcessBaseType _type)
    {
        type = _type;
    }
    
    
    void ProcessIsolated::sendAlert( samson::network::Alert& alert )
    {
        std::vector<size_t> delilahs = distribution_information.network->getDelilahIds();
        
        for ( size_t i = 0 ; i < delilahs.size() ; i++ )
        {
            
            Packet * p = new Packet( Message::Alert );
            
            p->message->mutable_alert()->CopyFrom( alert );
            p->message->set_delilah_component_id( (size_t)-1 );
            
            
            // Direction of this paket
            p->to.node_type = DelilahNode;
            p->to.id = delilahs[i];
            
            // Send packet
            distribution_information.network->send( p );
            
        }
        
        
        
        
    }
    
    
}
