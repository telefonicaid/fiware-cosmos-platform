

#include "ParserQueueTask.h"        // Own interface

#include "samson/module/ModulesManager.h"      // samson::module::ModulesManager
#include "samson/common/KVInputVector.h"        // samson::KVInputVector

namespace samson {
    namespace stream {
        

        
        void ParserQueueTask::generateKeyValues( KVWriter *writer )
        {
            // Get the operation
            Operation *operation = ModulesManager::shared()->getOperation( streamQueue->operation() );
            
            // Run the generator over the ProcessWriter to emit all key-values
            Parser *parser = (Parser*) operation->getInstance();
            
            if( !parser )
            {
                setUserError("Error getting an instance of this operation");
                return;
            }
            
            parser->environment = &environment;
            parser->tracer = this;
            parser->operationController = this;
            

            parser->init(writer);
            
            std::set< Block* >::iterator b;
            for ( b = blocks.begin() ; b != blocks.end() ; b++)
            {
                char *data = (*b)->getData();
                size_t size = (*b)->getSize();
                
                LM_M(("Stream Parsing a block of size %s", au::Format::string(size).c_str() ));
                
                parser->run( data , size ,  writer );

            }
            
            parser->finish(writer);
            
            // Detele the created instance
            delete parser;
            
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
            
            // Run the generator over the ProcessWriter to emit all key-values
            Map *map = (Map*) operation->getInstance();
            
            if( !map )
            {
                setUserError("Error getting an instance of this operation");
                return;
            }
            
            map->environment = &environment;
            map->tracer = this;
            map->operationController = this;
            
            map->init(writer);
            
            std::set< Block* >::iterator b;
            for ( b = blocks.begin() ; b != blocks.end() ; b++)
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
                        LM_M(("Stream Mapping a block of size %s hasg group %d %s ", au::Format::string( (*b)->getSize() ).c_str() , hg , info[hg].str().c_str() ));
                        
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
        
        
    }
}
