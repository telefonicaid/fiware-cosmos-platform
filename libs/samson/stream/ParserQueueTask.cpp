


#include "samson/module/ModulesManager.h"      // samson::module::ModulesManager
#include "samson/common/KVInputVector.h"        // samson::KVInputVector

#include "PopQueue.h"               // engine::PopQueue

#include "ParserQueueTask.h"        // Own interface

namespace samson {
    namespace stream {
        
#pragma mark ParserQueueTask
        
        // Get the required blocks to process
        void ParserQueueTask::getBlocks( BlockMatrix * input_matrix )
        {
            size_t total_size = 0 ;
            
            while( ( total_size < 100000000 ) )
            {                
                Block * block = input_matrix->extractFromChannel( 0 );

                if( !block )
                {
                    return;
                }

                
                total_size += block->getSize();
                matrix.add(0, block);
            }
            
        }
        
        void ParserQueueTask::generateKeyValues( KVWriter *writer )
        {
            // Get the operation
            Operation *operation = ModulesManager::shared()->getOperation( streamQueue->operation() );
            
            //LM_M(("Stream Parsing begin" ));

            // Get the list of blocks to process
            int channel = 0;
            BlockList* list = matrix.channels.findInMap( channel );
            
            if( !list ) 
                return;
            
            
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

            std::list< Block* >::iterator b;
            for ( b = list->blocks.begin() ; b != list->blocks.end() ; b++)
            {
                char *data = (*b)->getData();
                size_t size = (*b)->getSize();
                
                //LM_M(("Stream Parsing a block of size %s", au::Format::string(size).c_str() ));
                
                parser->run( data , size ,  writer );

            }
            
            parser->finish(writer);
            
            // Detele the created instance
            delete parser;

            //LM_M(("Stream Parsing finish" ));
            
        }
        
        std::string ParserQueueTask::getStatus()
        {
            std::ostringstream output;
            output << "[" << id << "] ";
            output << "Parser " << streamQueue->operation() << " processing " << matrix.str();
            return output.str();
        }
        

#pragma mark ParserOutQueueTask

        ParserOutQueueTask::ParserOutQueueTask( size_t id , PopQueue* pq  ) : stream::QueueTask(id , pq )
        {
            operation_name = "stream:" + pq->getParserOut();
        }
        
        std::string ParserOutQueueTask::getStatus()
        {
            std::ostringstream output;
            output << "[" << id << "] ";
            output << "Parserout " << streamQueue->operation() << " processing " << matrix.str();
            return output.str();
        }
        
        // Get the required blocks to process
        void ParserOutQueueTask::getBlocks( BlockList * input_list )
        {
            size_t total_size = 0 ;
            
            while( ( total_size < 100000000 ) )
            {                
                Block * block = input_list->extract(  );
                
                if( !block )
                {
                    return;
                }
                
                total_size += block->getSize();
                matrix.add(0, block);
            }
            
        }

        void ParserOutQueueTask::generateTXT( TXTWriter *writer )
        {
            LM_M(("Running a stream ParserOutQueueTask with %d blocks" , matrix.getNumBlocks()));

            // Get the operation
            Operation *operation = ModulesManager::shared()->getOperation( pq->getParserOut() );
            
            // Type of inputs ( for slecting key-values )
            std::vector<KVFormat> inputFormats =  operation->getInputFormats();
            
            if( inputFormats.size() != 1 )
            {
                LM_W(("Parser with more that one input... that is not valid!"));
                return;
            }
            
            KVInputVector inputVector( operation );
            
            
            // Get the list of blocks to process
            int channel = 0;
            BlockList* list = matrix.channels.findInMap( channel );
            
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
            
            std::list< Block* >::iterator b;
            for ( b = list->blocks.begin() ; b != list->blocks.end() ; b++)
            {
                
                Block* block = (*b);
                
                KVHeader *header = (KVHeader *)block->getData();
                
                if( !header->check() )
                {
                    LM_W(("Not valid header maping a block"));
                    return;
                }
                
                KVInfo* info = (KVInfo*) ( block->getData() + sizeof(KVHeader) );
                
                LM_M(("Stream Mapping a block of size %s with %s kvs in %s ", 
                      au::Format::string( (*b)->getSize() ).c_str() ,  
                      au::Format::string(header->info.kvs).c_str()  ,
                      au::Format::string(header->info.size,"Bytes").c_str()  
                      ));
                
                char *data = block->getData() + KVFILE_TOTAL_HEADER_SIZE;
                
                for (int hg = 0 ; hg < KVFILE_NUM_HASHGROUPS ; hg++)
                {
                    if( info[hg].size > 0 )
                    {
                        //LM_M(("Stream Mapping a block of size %s hasg group %d %s ", au::Format::string( (*b)->getSize() ).c_str() , hg , info[hg].str().c_str() ));
                        
                        inputVector.prepareInput( info[hg].kvs );
                        inputVector.addKVs( 0 , info[hg] , data );
                        
                        KVSetStruct inputStruct;
                        inputStruct.num_kvs = inputVector.num_kvs;
                        inputStruct.kvs = inputVector._kv;
                        
                        parserOut->run( &inputStruct , writer );
                        
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

        
        // Get the required blocks to process
        void MapQueueTask::getBlocks( BlockMatrix *input_matrix )
        {
            size_t total_size = 0 ;
            
            while( ( total_size < 100000000 ) )
            {                
                Block * block = input_matrix->extractFromChannel( 0 );
                
                if( !block )
                {
                    return;
                }
                
                
                total_size += block->getSize();
                matrix.add(0, block);
            }
            
        }
        
        void MapQueueTask::generateKeyValues( KVWriter *writer )
        {
            // Get the operation
            Operation *operation = ModulesManager::shared()->getOperation( streamQueue->operation() );

            // Type of inputs ( for slecting key-values )
            std::vector<KVFormat> inputFormats =  operation->getInputFormats();
            
            if( inputFormats.size() != 1 )
            {
                LM_W(("Map with more that one input... that is not valid!"));
                return;
            }
            
            KVInputVector inputVector( operation );
            
            
            // Get the list of blocks to process
            int channel = 0;
            BlockList* list = matrix.channels.findInMap( channel );
            
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
            
            std::list< Block* >::iterator b;
            for ( b = list->blocks.begin() ; b != list->blocks.end() ; b++)
            {
                
                Block* block = (*b);
                
                KVHeader *header = (KVHeader *)block->getData();
                
                if( !header->check() )
                {
                    LM_W(("Not valid header maping a block"));
                    return;
                }
                
                KVInfo* info = (KVInfo*) ( block->getData() + sizeof(KVHeader) );
                
                LM_M(("Stream Mapping a block of size %s with %s kvs in %s ", 
                        au::Format::string( (*b)->getSize() ).c_str() ,  
                        au::Format::string(header->info.kvs).c_str()  ,
                        au::Format::string(header->info.size,"Bytes").c_str()  
                      ));
                
                char *data = block->getData() + KVFILE_TOTAL_HEADER_SIZE;
                
                for (int hg = 0 ; hg < KVFILE_NUM_HASHGROUPS ; hg++)
                {
                    if( info[hg].size > 0 )
                    {
                        //LM_M(("Stream Mapping a block of size %s hasg group %d %s ", au::Format::string( (*b)->getSize() ).c_str() , hg , info[hg].str().c_str() ));
                        
                        inputVector.prepareInput( info[hg].kvs );
                        inputVector.addKVs( 0 , info[hg] , data );

                        KVSetStruct inputStruct;
                        inputStruct.num_kvs = inputVector.num_kvs;
                        inputStruct.kvs = inputVector._kv;
                        
                        map->run( &inputStruct , writer );

                        // Update the data pointer    
                        data +=  info[hg].size;
                        
                    }
                }
                 
                
            }
            
            map->finish(writer);
            
            // Detele the created instance
            delete map;
            
        }        

        std::string MapQueueTask::getStatus()
        {
            std::ostringstream output;
            output << "[" << id << "] ";
            output << "Map " << streamQueue->operation() << " processing " << matrix.str();
            return output.str();
        }
        
        
#pragma mark ReduceQueueTask
        
        class BlockReader
        {
            
        public:
            
            KVInfo *info;       // Pointer to the info data
            int current_hg;     // Current hash-group
            char *data;         // Pointer to the key-values
            int channel;        // Input channel associated to this block
            
            BlockReader( Block* block , int _channel )
            {
                info = (KVInfo *) ( block->getData() + sizeof( KVHeader ) );
                data = block->getData() + KVFILE_TOTAL_HEADER_SIZE;
                
                current_hg = 0;
                channel = _channel;
/*                
                LM_M(("New block reader"));
                for (int i = 0 ; i < KVFILE_NUM_HASHGROUPS ; i++)
                    if( info[i].kvs > 0 )
                        LM_M(("Information for hash-group %d -> %s" , i , info[i].str().c_str() ));
 */
                
            }
            
            void prepare ( int hg )
            {
                //LM_M(("Preparing block for hash-group %d", hg ));
                
                if( hg < current_hg )
                    LM_X(1, ("Invalid use of BlockReader"));
                
                while( current_hg < hg )
                {
                    //LM_M(("Skiping %lu bytes hg:%d->%d", info[current_hg].size , current_hg , hg));
                    data += info[current_hg].size;
                    current_hg++;
                }
            }            
            
        };
        
        class BlockReaderList
        {
            std::vector<BlockReader> blockReaders;
            
        public:

            KVInputVector inputVector;
            
            BlockReaderList( Operation*operation ) : inputVector( operation )
            {
                
            }
            
            void insert( Block *block , int channel )
            {
                blockReaders.push_back( BlockReader( block, channel ) );
            }
            
            size_t prepare( int hg )
            {
                //LM_M(("Preparing hash-group %d",hg));
                
                for ( size_t i = 0 ; i < blockReaders.size() ; i++)
                    blockReaders[i].prepare(hg);
                
                size_t num_kvs = 0 ;
                for ( size_t i = 0 ; i < blockReaders.size() ; i++)
                    num_kvs += blockReaders[i].info[hg].kvs;

                if ( num_kvs > 0 )
                {
                    inputVector.prepareInput( num_kvs );
                    
                    // Get data
                    for ( size_t i = 0 ; i < blockReaders.size() ; i++)
                        inputVector.addKVs( blockReaders[i].channel, blockReaders[i].info[hg],  blockReaders[i].data );
                }
                
                    //LM_M(("Finish Preparing hash-group %d with %lu key-values",hg, num_kvs));
                
                return num_kvs;
            }
            
        };
        
        
        void ReduceQueueTask::generateKeyValues( KVWriter *writer )
        {
            //LM_M(("Reducing from %d - %d" , hg_begin , hg_end ));
            
            // Get the operation
            Operation *operation = ModulesManager::shared()->getOperation( streamQueue->operation() );
            
            // Get the operation instance 
            Reduce *reduce = (Reduce*) operation->getInstance();
            
            reduce->environment = &operation_environment;
            reduce->tracer = this;
            reduce->operationController = this;
            
            reduce->init( writer );

             
            // Get the block reader list to prepare inputs for operation
            BlockReaderList blockReaderList( operation );

            // Put all the blocks inside...
            for (int i = 0 ; i < operation->getNumInputs() ; i++)
            {
                BlockList* list = matrix.channels.findInMap( i );

                if( list )
                {
                    std::list< Block* >::iterator b;
                    for ( b = list->blocks.begin() ; b != list->blocks.end() ; b++)
                        blockReaderList.insert( *b , i );
                }
            }
            
            // structure used to input reduce operations    
            int num_inputs = operation->getNumInputs();
            KVSetStruct* inputStructs = (KVSetStruct*) malloc( sizeof(KVSetStruct) * num_inputs );

            // compare functions necessary for sorting and grouping key-values
            KVInputVector &inputs = blockReaderList.inputVector;
            inputs.compare = operation->getInputCompareFunction();
            OperationInputCompareFunction compareKey = operation->getInputCompareByKeyFunction();
            
            
            for (int hg = 0 ; hg < KVFILE_NUM_HASHGROUPS ; hg++)
            {
                if( ( hg >= hg_begin ) && (hg < hg_end) )
                {
                    
                    // Prepare all the inputs for this hash.group
                    size_t num_kvs = blockReaderList.prepare(hg);
                    
                    if( num_kvs > 0 )
                    {
                        
                        // Sorting 
                        inputs.sort();
                        
                        //LM_M(("Running reduce over %lu kvs for hg [ %d in ( %d - %d ) ]", num_kvs , hg , hg_begin , hg_end ));
                        
                        // Run a reduce for each "key"
                        //std::cout << "Hash group with " << num_kvs << " kvs inputs sorted\n";
                        
                        // Process all the key-values in order
                        size_t pos_begin = 0;	// Position where the next group of key-values begin
                        size_t pos_end	 = 1;	// Position where the next group of key-values finish
                        
                        //std::cout << "Hash group with " << num_kvs << " kvs processing\n";
                        
                        /*
                         if( num_kvs > 0 )
                         std::cout << "Reduce Hash group with " << num_kvs << " kvs\n";
                         */
                        
                        while( pos_begin < num_kvs )
                        {
                            //std::cout << "PB: " << pos_begin << " PE: " << pos_end << "\n";
                            // Identify the number of key-values with the same key
                            while( ( pos_end < num_kvs ) && ( compareKey( inputs._kv[pos_begin] , inputs._kv[pos_end] ) == 0) )
                                pos_end++;
                            
                            size_t pos_pointer = pos_begin;
                            for (int i = 0 ; i < num_inputs ;i++)
                            {
                                if( (pos_pointer == pos_end) || ( inputs._kv[pos_pointer]->input != i) )
                                    inputStructs[i].num_kvs = 0;
                                else
                                {
                                    inputStructs[i].kvs = &inputs._kv[pos_pointer];
                                    inputStructs[i].num_kvs = 0;
                                    while( ( pos_pointer < pos_end ) && ( inputs._kv[pos_pointer]->input == i) )
                                    {
                                        inputStructs[i].num_kvs++;
                                        pos_pointer++;
                                    }
                                }
                            }
                            
                            reduce->run(inputStructs, writer);
                            
                            // Go to the next position
                            pos_begin = pos_end;
                            pos_end = pos_begin + 1;
                            
                            
                        }                        
                        
                    }
                    
                }
            }
            
            
            reduce->finish( writer  );
            
            // Detele the created instance
            delete reduce;            
            
            free( inputStructs ) ;
            
            //LM_M(("Finish Reducing from %d - %d" , hg_begin , hg_end ));
            
        }          

        
        
        std::string ReduceQueueTask:: getStatus()
        {
            std::ostringstream output;
            output << "[" << id << "] ";
            output << "Reduce " << streamQueue->operation() << " processing " << matrix.str();
            return output.str();
        }
        
        
        
    }
}
