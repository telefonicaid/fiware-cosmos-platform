

#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"

#include "engine/Object.h"                          // engine::Object
#include "engine/Notification.h"                    // engine::Notification

#include "samson/module/ModulesManager.h"           // samson::module::ModulesManager
#include "samson/common/KVInputVector.h"            // samson::KVInputVector
#include "samson/network/Packet.h"					// samson::Packet

#include "samson/common/NotificationMessages.h"     // Notifications
#include "samson/common/KVFile.h"
#include "PopQueue.h"                               // engine::PopQueue


#include "StreamManager.h"                          // StreamManager

#include "QueueTasks.h"                        // Own interface


namespace samson {
    namespace stream {
        
#pragma mark ParserQueueTask
        
        
        // Handy class to print traces on screen
        
        class OperationTraces
        {
            std::string name;
            au::Cronometer cronometer;
            size_t input_size;

        public:
            
            OperationTraces( std::string _name , size_t _input_size )
            {
                name = _name;
                input_size = _input_size;
                LM_T( LmtIsolatedOperations , ("%s starts with input %s" , name.c_str() , au::str( input_size ,"B" ).c_str() ));            
            }
            
            ~OperationTraces()
            {
                size_t time = cronometer.diffTimeInSeconds();
                double rate = 0;
                if( time > 0 )
                    rate = input_size / time;
                
                LM_T( LmtIsolatedOperations , ("%s ( input size %s ) finish atfer %s. Aprox rate %s" 
                                               , name.c_str() 
                                               , au::str( input_size ,"B" ).c_str()
                                               , cronometer.str().c_str()
                                               , au::str( rate , "B/s" ).c_str()
                                               ));            
            }
            
            void trace_block( size_t block_size )
            {
                LM_T( LmtIsolatedOperations , ("%s running a block of %s. Time since start %s" , name.c_str() , au::str( block_size , "B").c_str() , cronometer.str().c_str() ));
            }
            
        };
        
        
        void ParserQueueTask::generateKeyValues( samson::ProcessWriter *writer )
        {
            // Handy class to emit traces
            //OperationTraces operation_traces( au::str( "[%lu] Parser %s" , id,  operation_name.c_str() ) , getUniqueBlockInfo().size );
            OperationTraces operation_traces( au::str( "[%lu] Parser %s" , id,  operation_name.c_str() ) , 100 );


            
            // Get the operation
            Operation *operation = ModulesManager::shared()->getOperation( streamOperation->operation );

            if( !operation )
            {
              std::ostringstream error_message;
              error_message << "Unknown operation [" << id << "]  (" << streamOperation->operation << "; " << operation_name << ")";
              LM_E(("Error: %s", error_message.str().c_str()));

                setUserError(error_message.str());
                return;
            }
            
            
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
                
                // Trace 
                operation_traces.trace_block( b->getSize() );
                
                parser->run( data , size ,  writer );
                
            }
            
            parser->finish(writer);
            
            // Detele the created instance
            delete parser;
            
        }
        
        void ParserQueueTask::finalize( StreamManager* streamManager )
        {
            
            // Notify to the stream operation
            std::string stream_operation_name = environment.get("system.stream_operation","");
            StreamOperation* stream_operation = streamManager->getStreamOperation( stream_operation_name );
            if( stream_operation )
                stream_operation->remove( this );
            
        }    

        
        std::string ParserQueueTask::getStatus()
        {
            std::ostringstream output;
            output << "[" << id << "] ";
            output << "Parser " << streamOperation->operation;
            return output.str();
        }
        

#pragma mark ParserOutQueueTask
        
        std::string ParserOutQueueTask::getStatus()
        {
            std::ostringstream output;
            output << "[" << id << "] ";
            output << "Parserout " << streamOperation->operation;
            return output.str();
        }
        
        
        void ParserOutQueueTask::finalize( StreamManager* streamManager )
        {
            
            
            // Notify to the stream operation
            std::string stream_operation_name = environment.get("system.stream_operation","");
            StreamOperation* stream_operation = streamManager->getStreamOperation( stream_operation_name );
            if( stream_operation )
                stream_operation->remove( this );
            
            
        }    
        
        void ParserOutQueueTask::generateTXT( TXTWriter *writer )
        {
            // Handy class to emit traces
            //OperationTraces operation_traces( au::str( "[%lu] Parserout %s" , id,  operation_name.c_str() ) , getUniqueBlockInfo().size );
            OperationTraces operation_traces( au::str( "[%lu] Parserout %s" , id,  operation_name.c_str() ) , 100 );


            // Get the operation
            Operation *operation = ModulesManager::shared()->getOperation( streamOperation->operation );
            
            // Type of inputs ( for selecting key-values )
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

                // KV File to access content of this block
                KVFile file( block->getData() );


                if( file.isErrorActivated() )
                {
                    LM_W(("Not block(%s)" , file.getErrorMessage().c_str()) );
                    return;
                }
                
                for (int hg = 0 ; hg < KVFILE_NUM_HASHGROUPS ; hg++)
                {
                    KVInfo info = file.getKVInfoForHashGroup(hg);

                    if( info.size > 0 )
                    {
                        if( ( hg >= range.hg_begin ) && ( hg < range.hg_end) )
                        {
                            
                            //LM_M(("Stream Mapping a block of size %s hash group %d %s ", au::str( (*b)->getSize() ).c_str() , hg , info[hg].str().c_str() ));
                            
                            inputVector.prepareInput( info.kvs );
                            inputVector.addKVs( 0 , info , file.dataForHashGroup(hg) );
                            
                            KVSetStruct inputStruct;
                            inputStruct.num_kvs = inputVector.num_kvs;
                            inputStruct.kvs = inputVector._kv;
                            
                            parserOut->run( &inputStruct , writer );
                            
                        }
                        
                    }
                }
                
                
            }
            
            parserOut->finish(writer);
            
            // Detele the created instance
            delete parserOut;
            
            
        }
        
#pragma mark MapQueueTask
        
        void MapQueueTask::generateKeyValues( samson::ProcessWriter *writer )
        {
            // Handy class to emit traces
            //OperationTraces operation_traces( au::str( "[%lu] Map %s" , id,  operation_name.c_str() ) , getUniqueBlockInfo().size );
            OperationTraces operation_traces( au::str( "[%lu] Map %s" , id,  operation_name.c_str() ) , 100 );

            

            // Get the operation
            Operation *operation = ModulesManager::shared()->getOperation( streamOperation->operation );

            // Type of inputs ( for selecting key-values )
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
                
                //KVInfo* info = (KVInfo*) ( block->getData() + sizeof(KVHeader) );
                
                
                char *data = block->getData() + sizeof(KVHeader);


                inputVector.prepareInput( header->info.kvs );
                inputVector.addKVs( 0 , header->info , data );
                
                KVSetStruct inputStruct;
                inputStruct.num_kvs = inputVector.num_kvs;
                inputStruct.kvs = inputVector._kv;
                
                map->run( &inputStruct , writer );
                
            }
            
            map->finish(writer);
            
            // Detele the created instance
            delete map;
            
            
            //LM_M(("MAP Generate key-values DONE"));

            
        }        

        void MapQueueTask::finalize( StreamManager* streamManager )
        {
            
            // Notify to the stream operation
            std::string stream_operation_name = environment.get("system.stream_operation","");
            StreamOperation* stream_operation = streamManager->getStreamOperation( stream_operation_name );
            if( stream_operation )
                stream_operation->remove( this );
            
            
        }    
        
        std::string MapQueueTask::getStatus()
        {
            std::ostringstream output;
            output << "[" << id << "] ";
            output << "Map " << streamOperation->operation;
            return output.str();
        }
        
#pragma mark BlockReaderList
        
        // Class to process a block as input for a reduce operation
        
        class BlockReader
        {
            
            KVFile file;        // Internal management of a block
            int channel;        // Input channel associated to this block
            
        public:
            
            BlockReader( Block* block , int _channel ) :  file( block->buffer_container.getBuffer()->getData() )
            {
                // Assigned channel
                channel = _channel;
            }

            char* getDataForHashGroup( int hg )
            {
                return file.dataForHashGroup(hg); 
            }
            
            KVInfo getKVInfoForHashGroup( int hg )
            {
                return file.getKVInfoForHashGroup(hg);
            }
            
            int getChannel()
            {
                return channel;
            }
            
        };
        
        
        class BlockReaderCollection
        {
            
            // Collection of BlockReaders ( each for every block used as input for this operation )
            au::vector<BlockReader> inputBlockReaders;     
            
            // Only one state at the moment
            au::vector<BlockReader> stateBlockReaders;     
            
            // Common structure to give data to 3rd party software
            KVInputVector kvVector;
            
            size_t input_num_kvs;
            size_t state_num_kvs;
            
            Operation *operation;

        public:
            
            
            BlockReaderCollection( Operation *_operation ) : kvVector( _operation )
            {
                operation = _operation;
            }
            
            ~BlockReaderCollection()
            {
                inputBlockReaders.clearVector();
                stateBlockReaders.clearVector();
            }
            
            void insertInputBlocks( Block *block , int channel )
            {

                if ( ( channel < 0 ) || ( channel > ( operation->getNumInputs() - 1 ) ) )
                    LM_X(1,("Internal error"));
                
                inputBlockReaders.push_back( new BlockReader( block, channel ) );
            }

            void insertStateBlocks( Block *block , int channel )
            {
                if ( ( channel < 0 ) || ( channel > ( operation->getNumInputs() - 1 ) ) )
                    LM_X(1,("Internal error"));
                
                stateBlockReaders.push_back( new BlockReader( block, channel ) );
            }
            
            
            size_t prepare( int hg )
            {
                // Getting the number of key-values for input 
                input_num_kvs = 0 ;
                for ( size_t i = 0 ; i < inputBlockReaders.size() ; i++)
                    input_num_kvs += inputBlockReaders[i]->getKVInfoForHashGroup(hg).kvs;

                // Getting the number of key-values for input 
                state_num_kvs = 0 ;
                for ( size_t i = 0 ; i < stateBlockReaders.size() ; i++)
                    state_num_kvs += stateBlockReaders[i]->getKVInfoForHashGroup(hg).kvs;
                
                // Total number of key-values
                size_t total_kvs = input_num_kvs + state_num_kvs;
                
                // Prepare KV Vector with the total number of kvs ( from input and state )
                kvVector.prepareInput( total_kvs );

                // Add key values for all the inputs
                if ( input_num_kvs > 0 )
                {
                    for ( size_t i = 0 ; i < inputBlockReaders.size() ; i++)
                        kvVector.addKVs( inputBlockReaders[i]->getChannel()
                                        , inputBlockReaders[i]->getKVInfoForHashGroup(hg) 
                                        , inputBlockReaders[i]->getDataForHashGroup(hg) 
                                        );
                }

                // Add key values for the state
                if ( state_num_kvs > 0 )
                {
                    for ( size_t i = 0 ; i < stateBlockReaders.size() ; i++)
                        kvVector.addKVs( stateBlockReaders[i]->getChannel()
                                        , stateBlockReaders[i]->getKVInfoForHashGroup(hg)
                                        , stateBlockReaders[i]->getDataForHashGroup(hg) 
                                        );
                }
                
                return total_kvs;

            }                
            
            
            void sortAndInit()
            {
                if( state_num_kvs == 0 )    // No state
                {
                    //LM_M(("Sorting %lu key-values" , input_num_kvs));
                    kvVector.sort();
                }
                else if( input_num_kvs > 0 )
                {
                    //LM_M(("Merge-Sorting %lu / %lu key-values" , input_num_kvs , state_num_kvs));
                    kvVector.sortAndMerge(input_num_kvs);
                }
                
                kvVector.init();
            }
            
            KVSetStruct* getNext()
            {
                return kvVector.getNext();
            }
            
        };
        

#pragma mark

        
        ReduceQueueTask::ReduceQueueTask( size_t id , StreamOperationBase* streamOperation , KVRange _range  ) 
            : stream::QueueTask(id , streamOperation )
        {
            setProcessItemOperationName( "stream:" + streamOperation->operation );
            
            //Range of activity
            range = _range;
            
            // By default it is not in update_state mode
            update_state_mode = false;

            // By default it is not a update_only operation
            update_only = false;
            
        }
        
        void ReduceQueueTask::set_update_only()
        {
            update_only = true;
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
        
        
        void ReduceQueueTask::generateKeyValues( samson::ProcessWriter *writer )
        {
            if( update_only )
                LM_T(LmtReduceOperation, ("Reduce %lu - %s begin.... (update_only)" , id , operation_name.c_str() ));
            else
                LM_T(LmtReduceOperation, ("Reduce %lu - %s begin" , id , operation_name.c_str() ));
            
            LM_T(LmtReduceOperation, ("Reduce %lu - %s KVRange %s" , id , operation_name.c_str() , range.str().c_str() ));
            
            // Handy class to emit traces
            //OperationTraces operation_traces( au::str( "[%lu] reduce %s" , id,  operation_name.c_str() ) , getUniqueBlockInfo().size );
            OperationTraces operation_traces( au::str( "[%lu] reduce %s" , id,  operation_name.c_str() ) , 100 );

            
            // Goyo Why do we make this update?
            // deadlock problem detected, not starting operations
            //BlockInfo block_info;
            //update( block_info );
            
            // Get the operation
            Operation *operation = ModulesManager::shared()->getOperation( streamOperation->operation );
            
            // Get the operation instance 
            Reduce *reduce = (Reduce*) operation->getInstance();
            
            // Prepare the operation
            reduce->environment = &operation_environment;
            reduce->tracer = this;
            reduce->operationController = this;
            
            // Init function
            reduce->init( writer );
            
            LM_T(LmtReduceOperation, ("Reduce %lu - %s Preparing data...(%s)" 
                                      , id , operation_name.c_str() , getBlockListContainerDataDescription().c_str() ));

            // Get the block reader list to prepare inputs for operation
            BlockReaderCollection blockreaderCollection( operation );
            
            // Insert all the blocks involved in this operation
            for ( int i = 0 ; i < operation->getNumInputs() ; i++ )
            {
                BlockList* list = getBlockList( au::str("input_%d", i) );
                std::list< Block* >::iterator b;
                
                
                for ( b = list->blocks.begin() ; b != list->blocks.end() ; b++ )
                {
                    
                    if( update_state_mode && (i == (operation->getNumInputs()-1)) )
                        blockreaderCollection.insertStateBlocks( *b , i );
                    else
                        blockreaderCollection.insertInputBlocks( *b , i );
                }
            }
           
            // Counters for statistics
            int total_hgs =  range.hg_end - range.hg_begin;
            int processed_hds = 0;
            
            int transfered_states = 0;
            int processed_states = 0;
            
            LM_T(LmtReduceOperation, ("Reduce %lu - %s Data ready! Running hash-groups" , id , operation_name.c_str() ));
            
            for (int hg = 0 ; hg < KVFILE_NUM_HASHGROUPS ; hg++)
            {
                
                // Check if this is inside the range we are interested in processing
                if( ( hg >= range.hg_begin ) && (hg < range.hg_end) )
                {

                    // Report progress to show in the lists
                    processed_hds++;
                    reportProgress( (double)processed_hds / (double)total_hgs );
                    
                    // Prepare the blockReaderCollection for thi hash-group
                    size_t num_kvs = blockreaderCollection.prepare( hg );
                    
                    // sort input key-values and init the kvVector structure
                    blockreaderCollection.sortAndInit();
                    
                    if( num_kvs > 0 )
                    {
                        
                        // Get data for the next reduce operation
                        KVSetStruct* inputStructs = blockreaderCollection.getNext();
                        
                        while( inputStructs )
                        {
                            
                            if( update_only && (inputStructs[0].num_kvs == 0) )
                            {
                                
                                // Just copy state to the output...
                                if( inputStructs[1].num_kvs > 0 ) // Check we really have state
                                {
                                    // Satte out
                                    int state_output = operation->getNumOutputs()-1;
                                    
                                    // Data pointer
                                    char * key_data = inputStructs[1].kvs[0]->key;
                                    //char * value_data = inputStructs[1].kvs[0]->value;
                                    
                                    // Total size
                                    size_t key_size = inputStructs[1].kvs[0]->key_size;
                                    size_t value_size = inputStructs[1].kvs[0]->value_size;

                                    //inputStructs[1].kvs[0]->key
                                    writer->internal_emit(state_output, hg , key_data, key_size + value_size );
                                    transfered_states++;
                                }
                                else
                                    LM_T(LmtReduceOperation, ("Reduce %lu - %s ERROR since no data at input nor state" 
                                                              , id 
                                                              , operation_name.c_str()
                                                              ));
                                    
                                
                            }
                            else
                            {
                            
                                // Call the reduce operation
                                reduce->run( inputStructs, writer );
                                processed_states++;
                            }
                            
                            // Get the next one
                            inputStructs = blockreaderCollection.getNext();
                        }
                        
                    }
                }
                

            }
            
            reduce->finish( writer  );
            
            // Detele the created instance
            delete reduce;            
            
            //LM_M(("Finish ReduceQueueTask for range %s  [%s]" , range.str().c_str() , block_info.str().c_str() ));
            LM_T(LmtReduceOperation, ("Reduce %lu - %s finish [ Different key Processed: %d DirectState: %d ]" 
                                      , id 
                                      , operation_name.c_str()
                                      , processed_states
                                      , transfered_states
                                      ));
            
        }          
        
        std::string ReduceQueueTask::getStatus()
        {
            std::ostringstream output;
                output << "[" << id << "] ";
            output << "Reduce " << streamOperation->operation;
            return output.str();
        }

        void ReduceQueueTask::processOutputBuffer( engine::Buffer *buffer , int output , int outputWorker , bool finish )
        {
            finish = false;

            int output_channel = (int)streamOperation->output_queues.size() - 1 ;
             // Special case for logging
             LM_T(LmtIsolatedOutputs, ("Checking output(%d) with output_queues.size(%d) to send logging for worker:%d", output, (int) streamOperation->output_queues.size(), outputWorker));
             if( output == (int) streamOperation->output_queues.size() )
             {
             	LM_T(LmtIsolatedOutputs, ("Sending buffer to log_queue for stream_operation(%s)", streamOperation->name.c_str()));
                 sendBufferToQueue( buffer , outputWorker , au::str("log_%s", streamOperation->name.c_str() ) );
                 return;
             }
            
            
            if( update_state_mode && ( output == output_channel ) )
            {
                outputBuffers.push_back( buffer );
            }
            else
            {
                // Exception to send logs
                if( output == (int) streamOperation->output_queues.size() )
                {
                    sendBufferToQueue( buffer , outputWorker , au::str("log_%s", streamOperation->name.c_str() ) );
                    return;
                }

                sendBufferToQueue( buffer , outputWorker , streamOperation->output_queues[output] );
            }
        }
        
        void ReduceQueueTask::finalize( StreamManager* streamManager )
        {
            if( !update_state_mode )
            {
                // Notify to the stream operation
                std::string stream_operation_name = environment.get("system.stream_operation","");
                StreamOperation* stream_operation = streamManager->getStreamOperation( stream_operation_name );
                
                if( stream_operation )
                    stream_operation->remove( this );
                
                return;
            }
            
            // Create the list with the outputs
            BlockList tmp("ReduceQueueTask_outputs");
            while( outputBuffers.getNumBuffers() > 0 )
            {
                engine::Buffer* buffer = outputBuffers.front();
                tmp.createBlock( buffer );
                outputBuffers.pop();
            }
            
            // Original list of blocks
            BlockList *originalBlockList = getBlockList("input_1");
            
            // Get the queue we are working on
            std::string queue_name = streamOperation->output_queues[ (int)streamOperation->output_queues.size() - 1 ];
            Queue*queue = streamManager->getQueue( queue_name );
            
            //LM_M(("Running finalize of a reduce for queue %s" , queue_name.c_str() ));
            
            if ( queue )
                queue->replaceAndUnlock( originalBlockList , &tmp );
            
            // Notify to the stream operation
            std::string stream_operation_name = environment.get("system.stream_operation","");
            StreamOperation* stream_operation = streamManager->getStreamOperation( stream_operation_name );

            if( stream_operation )
            {
                stream_operation->remove( this );
                
                StreamOperationUpdateState * stream_operation_update_state =  (StreamOperationUpdateState*) stream_operation;
                if( !stream_operation_update_state )
                    LM_W(("Error reporting that division %d finished" , division ));
                else
                    stream_operation_update_state->finish_update_division( division );

            }
            
        }           
        

    }
}
