


#include "engine/Object.h"                          // engine::Object
#include "engine/Notification.h"                    // engine::Notification

#include "samson/module/ModulesManager.h"           // samson::module::ModulesManager
#include "samson/common/KVInputVector.h"            // samson::KVInputVector
#include "samson/network/Packet.h"					// samson::Packet

#include "samson/common/NotificationMessages.h"     // Notifications

#include "PopQueue.h"                               // engine::PopQueue


#include "StreamManager.h"                          // StreamManager

#include "QueueTasks.h"                        // Own interface


namespace samson {
    namespace stream {
        
#pragma mark ParserQueueTask
        
        
        void ParserQueueTask::generateKeyValues( KVWriter *writer )
        {
            // Get the operation
            Operation *operation = ModulesManager::shared()->getOperation( streamOperation->operation() );
            
            //LM_M(("Stream Parsing begin" ));
            
            // Run the generator over the ProcessWriter to emit all key-values
            Parser *parser = (Parser*) operation->getInstance();
            
            if( !parser )
            {
                setUserError("Error getting an instance of this operation");
                return;
            }
            
            parser->environment = &operation_environment;
            parser->tracer = this;
            parser->operationController = this;

            parser->init(writer);

            BlockList *list = getBlockList( "input_0" );
            au::list< Block >::iterator bi;
            for ( bi = list->blocks.begin() ; bi != list->blocks.end() ; bi++)
            {
                
                Block *b = *bi;
                
                char *data = b->getData() + sizeof( KVHeader );
                size_t size = b->getSize() - sizeof( KVHeader );
                
                //LM_M(("Stream Parsing a block of size %s", au::str(size).c_str() ));
                
                parser->run( data , size ,  writer );
                
            }
            
            parser->finish(writer);
            
            // Detele the created instance
            delete parser;

            //LM_M(("Stream Parsing finish" ));
            
        }
        
        void ParserQueueTask::finalize( StreamManager* streamManager )
        {
            // Original list of blocks
            BlockList *input = getBlockList("input_0");
            
            // Get the queue we are working on
            std::string queue_name = streamOperation->input_queues( 0 );
            Queue*queue = streamManager->getQueue( queue_name );

            if ( queue )
            {
                queue->unlock( input );
                
                // If specified, remove input blocks
                if( environment.get("system.clear_imputs" , "no") == "yes" )
                    queue->remove( input );

                
            }
        }    

        
        std::string ParserQueueTask::getStatus()
        {
            std::ostringstream output;
            output << "[" << id << "] ";
            output << "Parser " << streamOperation->operation();
            return output.str();
        }
        

#pragma mark ParserOutQueueTask
        
        std::string ParserOutQueueTask::getStatus()
        {
            std::ostringstream output;
            output << "[" << id << "] ";
            output << "Parserout " << streamOperation->operation();
            return output.str();
        }
        
        
        void ParserOutQueueTask::finalize( StreamManager* streamManager )
        {
            
            // Original list of blocks
            BlockList *input = getBlockList("input_0");
            
            // Get the queue we are working on
            std::string queue_name = streamOperation->input_queues( 0 );
            Queue*queue = streamManager->getQueue( queue_name );
            
            if ( queue )
            {
                queue->unlock( input );
                
                // If specified, remove input blocks
                if( environment.get("system.clear_imputs" , "no") == "yes" )
                    queue->remove( input );
                
                
            }
            
        }    
        
        void ParserOutQueueTask::generateTXT( TXTWriter *writer )
        {

            // Get the operation
            Operation *operation = ModulesManager::shared()->getOperation( streamOperation->operation() );
            
            // Type of inputs ( for slecting key-values )
            std::vector<KVFormat> inputFormats =  operation->getInputFormats();
            
            if( inputFormats.size() != 1 )
            {
                LM_W(("Parser with more that one input... that is not valid!"));
                return;
            }
            
            KVInputVector inputVector( operation );
            
            
            // Run the generator over the ProcessWriter to emit all key-values
            ParserOut *parserOut = (ParserOut*) operation->getInstance();
            
            if( !parserOut )
            {
                setUserError("Error getting an instance of this operation");
                return;
            }
            
            parserOut->environment = &operation_environment;
            parserOut->tracer = this;
            parserOut->operationController = this;
            
            parserOut->init(writer);
            
            BlockList *list = getBlockList( "input_0" );
           au::list< Block >::iterator bi;
            for ( bi = list->blocks.begin() ; bi != list->blocks.end() ; bi++)
            {
                
                Block *block = *bi;
                
                KVHeader *header = (KVHeader *)block->getData();
                
                if( !header->check() )
                {
                    LM_W(("Not valid header maping a block"));
                    return;
                }
                
                KVInfo* info = (KVInfo*) ( block->getData() + sizeof(KVHeader) );
                
                LM_M(("Stream Mapping a block of size %s with %s kvs in %s ", 
                      au::str( block->getSize() ).c_str() ,  
                      au::str(header->info.kvs).c_str()  ,
                      au::str(header->info.size,"Bytes").c_str()  
                      ));
                
                char *data = block->getData() + KVFILE_TOTAL_HEADER_SIZE;
                
                for (int hg = 0 ; hg < KVFILE_NUM_HASHGROUPS ; hg++)
                {
                    
                    if( info[hg].size > 0 )
                    {
                        
                        if( ( hg >= range.hg_begin ) && ( hg < range.hg_end) )
                        {
                            
                            //LM_M(("Stream Mapping a block of size %s hasg group %d %s ", au::str( (*b)->getSize() ).c_str() , hg , info[hg].str().c_str() ));
                            
                            inputVector.prepareInput( info[hg].kvs );
                            inputVector.addKVs( 0 , info[hg] , data );
                            
                            KVSetStruct inputStruct;
                            inputStruct.num_kvs = inputVector.num_kvs;
                            inputStruct.kvs = inputVector._kv;
                            
                            parserOut->run( &inputStruct , writer );
                            
                        }
                        
                        // Update the data pointer    
                        data +=  info[hg].size;
                        
                    }
                }
                
                
            }
            
            parserOut->finish(writer);
            
            // Detele the created instance
            delete parserOut;
            
            
        }
        
#pragma mark MapQueueTask
        
        void MapQueueTask::generateKeyValues( KVWriter *writer )
        {
            // Get the operation
            Operation *operation = ModulesManager::shared()->getOperation( streamOperation->operation() );

            // Type of inputs ( for slecting key-values )
            std::vector<KVFormat> inputFormats =  operation->getInputFormats();
            
            if( inputFormats.size() != 1 )
            {
                LM_W(("Map with more that one input... that is not valid!"));
                return;
            }
            
            KVInputVector inputVector( operation );
            
            
            
            // Run the generator over the ProcessWriter to emit all key-values
            Map *map = (Map*) operation->getInstance();
            
            if( !map )
            {
                setUserError("Error getting an instance of this operation");
                return;
            }
            
            map->environment = &operation_environment;
            map->tracer = this;
            map->operationController = this;
            
            map->init(writer);

            BlockList *list = getBlockList( "input_0" );
            au::list< Block >::iterator bi;
            for ( bi = list->blocks.begin() ; bi != list->blocks.end() ; bi++)
            {
                
                Block *block = *bi;
                
                KVHeader *header = (KVHeader *)block->getData();
                
                if( !header->check() )
                {
                    LM_W(("Not valid header maping a block"));
                    return;
                }
                
                KVInfo* info = (KVInfo*) ( block->getData() + sizeof(KVHeader) );
                
                
                char *data = block->getData() + KVFILE_TOTAL_HEADER_SIZE;
                
                for (int hg = 0 ; hg < KVFILE_NUM_HASHGROUPS ; hg++)
                {
                    if( info[hg].size > 0 )
                    {
                        if( ( hg >= range.hg_begin ) || ( hg < range.hg_end) )
                        {
                            
                            inputVector.prepareInput( info[hg].kvs );
                            inputVector.addKVs( 0 , info[hg] , data );
                            
                            KVSetStruct inputStruct;
                            inputStruct.num_kvs = inputVector.num_kvs;
                            inputStruct.kvs = inputVector._kv;
                            
                            map->run( &inputStruct , writer );
                            
                        }
                        
                        // Update the data pointer    
                        data +=  info[hg].size;
                        
                    }
                }
                
                
            }
            
            map->finish(writer);
            
            // Detele the created instance
            delete map;
            
        }        

        void MapQueueTask::finalize( StreamManager* streamManager )
        {
            
            // Original list of blocks
            BlockList *input = getBlockList("input_0");
            
            // Get the queue we are working on
            std::string queue_name = streamOperation->input_queues( 0 );
            Queue*queue = streamManager->getQueue( queue_name );
            
            if ( queue )
            {
                queue->unlock( input );
                
                // If specified, remove input blocks
                if( environment.get("system.clear_imputs" , "no") == "yes" )
                    queue->remove( input );
                
                
            }
            
        }    
        
        std::string MapQueueTask::getStatus()
        {
            std::ostringstream output;
            output << "[" << id << "] ";
            output << "Map " << streamOperation->operation();
            return output.str();
        }
        
#pragma mark BlockReaderList
        
        // Class to process a block as input for a reduce operation
        
        class BlockReader
        {
            
        public:
            
            KVFile file;        // Internal management of a block

            int channel;        // Input channel associated to this block
            int current_hg;     // Current hash-group
            
            char *data;         // Pointer used to acess next data
            
            BlockReader( Block* block , int _channel ) :  file( block->buffer->getData() )
            {
                // Asigned channel
                channel = _channel;

                // We start always at hash-group "0"
                data = file.data;
                current_hg = 0;
            }
            
            void prepare ( int hg )
            {
                //LM_M(("Preparing block for hash-group %d", hg ));
                
                if( hg < current_hg )
                    LM_X(1, ("Invalid use of BlockReader"));
                
                while( current_hg < hg )
                {
                    //LM_M(("Skiping %lu bytes hg:%d->%d", info[current_hg].size , current_hg , hg));
                    data += file.info[current_hg].size;
                    current_hg++;
                }
            }            
            
        };
        
        
        class BlockReaderCollection
        {

        public:
            
            std::vector<BlockReader> blockReaders;
            KVInputVector kvVector;
            
            Operation *operation;
            
            BlockReaderCollection( Operation *_operation ) : kvVector( _operation )
            {
                operation = _operation;
            }
            
            void insert( Block *block , int channel )
            {
                if ( ( channel < 0 ) || ( channel > ( operation->getNumInputs() - 1 ) ) )
                    LM_X(1,("Internal error"));
                
                blockReaders.push_back( BlockReader( block, channel ) );
            }
            
            size_t prepare( int hg )
            {
                // Putting data in place
                for ( size_t i = 0 ; i < blockReaders.size() ; i++)
                    blockReaders[i].prepare(hg);

                // Getting the number of key-values
                size_t num_kvs = 0 ;
                for ( size_t i = 0 ; i < blockReaders.size() ; i++)
                    num_kvs += blockReaders[i].file.info[hg].kvs;

                // Prepare KV Vector with the total number of kvs
                kvVector.prepareInput( num_kvs );
                
                if ( num_kvs > 0 )
                {
                    // Get data
                    for ( size_t i = 0 ; i < blockReaders.size() ; i++)
                        kvVector.addKVs( blockReaders[i].channel, blockReaders[i].file.info[hg],  blockReaders[i].data );
                }
                
                return num_kvs;
            }                
            
            
            void sortAndInit()
            {
                kvVector.sort();
                kvVector.init();
            }
            
        };
        

#pragma mark

        
        ReduceQueueTask::ReduceQueueTask( size_t id , const network::StreamOperation& streamOperation , KVRange _range  ) 
        : stream::QueueTask(id , streamOperation )
        {
            setProcessItemOperationName( "stream:" + streamOperation.operation() );
            
            //Range of activity
            range = _range;
            
            // By default it is not in update_state mode
            update_state_mode = false;
            
        }
        
        void ReduceQueueTask::setUpdateStateDivision( int _division )
        {
            update_state_mode = true;
            division = _division;
        }
        
        
        // Structure to decide where to take key-values
        enum kvs_source
        {
            undefined,
            input_only,
            state_only,
            input_and_state
        };
        
        
        void ReduceQueueTask::generateKeyValues( KVWriter *writer )
        {
            BlockInfo block_info;
            update( block_info );
            
            // Get the operation
            Operation *operation = ModulesManager::shared()->getOperation( streamOperation->operation() );
            
            // Get the operation instance 
            Reduce *reduce = (Reduce*) operation->getInstance();
            
            // Prepare the operation
            reduce->environment = &operation_environment;
            reduce->tracer = this;
            reduce->operationController = this;
            
            // Init function
            reduce->init( writer );
            
            // Get the block reader list to prepare inputs for operation
            BlockReaderCollection blockreaderCollection( operation );

            // Insert all the blocks involved in this operation
            for ( int i = 0 ; i < operation->getNumInputs() ; i++ )
            {
                BlockList* list = getBlockList( au::str("input_%d", i) );
                std::list< Block* >::iterator b;
                for ( b = list->blocks.begin() ; b != list->blocks.end() ; b++)
                    blockreaderCollection.insert( *b , i );
            }
            
            for (int hg = 0 ; hg < KVFILE_NUM_HASHGROUPS ; hg++)
            {
                // Check if this is inside the range we are interested in processing
                if( ( hg >= range.hg_begin ) && (hg < range.hg_end) )
                {
                    // Prepare the blockReaderCollection for thi hash-group
                    size_t num_kvs = blockreaderCollection.prepare( hg );
                    
                    // sort input key-values and init the kvVector structure
                    blockreaderCollection.sortAndInit();
                    
                    if( num_kvs > 0 )
                    {
                        
                        // Get data for the next reduce operation
                        KVSetStruct* inputStructs = blockreaderCollection.kvVector.getNext();
                        
                        while( inputStructs )
                        {
                            // Call the reduce operation
                            reduce->run( inputStructs, writer );
                            
                            // Get the next one
                            inputStructs = blockreaderCollection.kvVector.getNext();
                        }
                        
                    }
                    
                }
            }
            reduce->finish( writer  );
            
            // Detele the created instance
            delete reduce;            
            
            //LM_M(("Finish ReduceQueueTask for range %s  [%s]" , range.str().c_str() , block_info.str().c_str() ));
            
        }          
        
        std::string ReduceQueueTask::getStatus()
        {
            std::ostringstream output;
                output << "[" << id << "] ";
            output << "Reduce " << streamOperation->operation();
            return output.str();
        }

        void ReduceQueueTask::processOutputBuffer( engine::Buffer *buffer , int output , int outputWorker , bool finish )
        {
            int output_channel = streamOperation->output_queues_size() - 1 ;
            
            //LM_M(("processOutputBuffer %d ( %d )" , output , output_channel ));
            
            if( update_state_mode && ( output == output_channel ) )
            {
                outputBuffers.push_back( buffer );
            }
            else
                sendBufferToQueue( buffer , outputWorker , streamOperation->output_queues(output) );
        }
        
        void ReduceQueueTask::finalize( StreamManager* streamManager )
        {
            
            // Create the list with the outputs
            BlockList *tmp = new BlockList("ReduceQueueTask_outputs");
            for (size_t i = 0 ; i < outputBuffers.size() ; i++ )
                tmp->createBlock( outputBuffers[i] );
            
            // Original list of blocks
            BlockList *originalBlockList = getBlockList("input_1");
            
            // Get the queue we are working on
            std::string queue_name = streamOperation->output_queues( streamOperation->output_queues_size() - 1 );
            Queue*queue = streamManager->getQueue( queue_name );
            
            if ( queue )
            {
                queue->replace( originalBlockList , tmp );
                
                // Release division we were processing with this operation
                queue->unlockDivision( division );
            }
            
            // Detele the temporal list used here
            delete tmp;
        }           
        

        
#pragma mark ReduceQueueTask
 /*       
        ReduceQueueTask::ReduceQueueTask( size_t id , const network::StreamOperation& streamOperation , QueueItem * _queueItem , KVRange _range  ) 
        : stream::QueueTask(id , streamOperation )
        {
            setProcessItemOperationName( "stream:" + streamOperation.operation() );
            
            //Range of activity
            range = _range;
            
            // Pointer to the state item
            queueItem = _queueItem;
        }
        
        // Get the required blocks to process
        void ReduceQueueTask::getBlocks( BlockList *input , BlockList * _state )
        {
            // Extract necessary blocks
            getBlockList( "input_0" )->extractFrom ( input , 100000000 );
            
            // copy all the blocks from the previous state
            getBlockList( "input_1" )->copyFrom( _state );
        }
                

        // Structure to decide where to take key-values
        enum kvs_source
        {
            undefined,
            input_only,
            state_only,
            input_and_state
        };
        
        
        void ReduceQueueTask::generateKeyValues( KVWriter *writer )
        {
            
            //LM_M(("Reducing ( HG %d - %d )" , stateItem->hg_begin , stateItem->hg_end ));
            //LM_M(("Reducing input %s", list->getFullKVInfo().str().c_str() ));
            //LM_M(("Reducing state %s", state->getFullKVInfo().str().c_str() ));
            
            // Get the operation
            Operation *operation = ModulesManager::shared()->getOperation( streamOperation->operation() );
            
            // Get the operation instance 
            Reduce *reduce = (Reduce*) operation->getInstance();

            // Prepare the operation
            reduce->environment = &operation_environment;
            reduce->tracer = this;
            reduce->operationController = this;

            // Init function
            reduce->init( writer );
            
            // Get the block reader list to prepare inputs for operation
            BlockReaderSystem blockReaderSystem( operation );
            
            // Insert all the blocks involved in this operation
            {
                BlockList* list = getBlockList( "input_0" );
                std::list< Block* >::iterator b;
                for ( b = list->blocks.begin() ; b != list->blocks.end() ; b++)
                    blockReaderSystem.insertInput( *b , 0 );
            }
            
            // Put all the blocks from state
            {
                BlockList* state = getBlockList( "input_1" );
                std::list< Block* >::iterator b;
                for ( b = state->blocks.begin() ; b != state->blocks.end() ; b++)
                    blockReaderSystem.insertState( *b );
            }
            
            
            // Structure used to input reduce operations    
            int num_inputs = operation->getNumInputs();
            KVSetStruct* inputStructs = (KVSetStruct*) malloc( sizeof(KVSetStruct) * num_inputs );
            
            
            // Function used to compare keys
            OperationInputCompareFunction compareKey = operation->getInputCompareByKeyFunction();

            
            for (int hg = 0 ; hg < KVFILE_NUM_HASHGROUPS ; hg++)
            {
                
                if( ( hg >= range.hg_begin ) && (hg < range.hg_end) )
                {
                    
                    
                    // Prepare all the inputs for this hash.group
                    size_t num_kvs = blockReaderSystem.prepare( hg );
                    
                    if( num_kvs > 0 )
                    {
                        
                        
                        size_t num_kvs_input = blockReaderSystem.input.kvVector.num_kvs;
                        size_t num_kvs_state = blockReaderSystem.state.kvVector.num_kvs;

                        //LM_M(("Processing hash-group %d :  %lu key-values ( %lu / %lu ) " , hg , num_kvs , num_kvs_input  , num_kvs_state ));
                        
                        KVInputVector &inputVector = blockReaderSystem.input.kvVector;
                        KVInputVector &stateVector = blockReaderSystem.state.kvVector;
                        
                        
                        size_t pos_input = 0;
                        size_t pos_state = 0;

                        
                        while ( ( pos_input < num_kvs_input ) || ( pos_state < num_kvs_state) )
                        {
                            // Let's decide where we will take key-values from ( input / state / both )
                            // ---------------------------------------------------------------------------------------
                            kvs_source source = undefined;
                            
                            if( pos_input >= num_kvs_input )
                                source = state_only;
                            else if( pos_state >= num_kvs_state )
                                source = input_only;
                            else
                            {
                                // We compare both first keys to decide source
                                int res = compareKey( inputVector._kv[pos_input] , stateVector._kv[pos_state] );
                                
                                if ( res == 0)
                                    source = input_and_state;
                                else if (res < 0)
                                    source = input_only;
                                else
                                    source = state_only;
                            }

                            // Based on the current sources , let's prepare the vector
                            // ---------------------------------------------------------------------------------------
                            
                            if( source == undefined )
                                LM_X(1,("Internal error"));
                            
                            if( ( source == input_only ) || ( source == input_and_state) )
                            {
                                inputStructs[0].kvs     = &inputVector._kv[pos_input];
                                inputStructs[0].num_kvs = inputVector.getNumKeyValueWithSameKey(pos_input);
                            }
                            else
                            {
                                inputStructs[0].kvs = NULL;
                                inputStructs[0].num_kvs = 0;
                            }

                            if( ( source == state_only ) || ( source == input_and_state) )
                            {
                                inputStructs[1].kvs     = &stateVector._kv[pos_state];
                                inputStructs[1].num_kvs = stateVector.getNumKeyValueWithSameKey(pos_state);
                            }
                            else
                            {
                                inputStructs[1].kvs = NULL;
                                inputStructs[1].num_kvs = 0;
                            }
                            
                            // Call the reduce operation
                            // ---------------------------------------------------------------------------------------
                            
                            reduce->run( inputStructs, writer );
                            
                            // Update the position of both pointers
                            // ---------------------------------------------------------------------------------------
                            
                            pos_input += inputStructs[0].num_kvs;
                            pos_state += inputStructs[1].num_kvs;
                            

                        }

                    }
                    
                }
            }
            
            
            reduce->finish( writer  );
            
            // Detele the created instance
            delete reduce;            
            
            free( inputStructs ) ;
            
            //LM_M(("Finish Reducing from %d - %d" , stateItem->hg_begin , stateItem->hg_end ));
            
        }          
        
        std::string ReduceQueueTask:: getStatus()
        {
            std::ostringstream output;
            output << "[" << id << "] ";
            output << "Reduce " << streamOperation->operation();
            return output.str();
        }
        
        void ReduceQueueTask::processOutputBuffer( engine::Buffer *buffer , int output , int outputWorker , bool finish )
        {
            
            
            if( output == ( streamOperation->output_queues_size() - 1 ) )
            {
                queueItem->push( this, buffer );
                return;
            }
            
            // Send a packet to be push in a queue ( normal behaviour in stream operations )
            sendBufferToQueue( buffer , outputWorker , streamOperation->output_queues(output) );
            
            
        }
        
        void ReduceQueueTask::finalize()
        {
            queueItem->notifyFinishOperation( this );
        };
*/        
        
    }
}
