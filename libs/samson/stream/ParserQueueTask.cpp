

#include "ParserQueueTask.h"        // Own interface

#include "samson/module/ModulesManager.h"      // samson::module::ModulesManager

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
            

            parser->init();
            
            std::set< Block* >::iterator b;
            for ( b = blocks.begin() ; b != blocks.end() ; b++)
            {
                char *data = (*b)->getData();
                size_t size = (*b)->getSize();
                
                LM_M(("Stream Parsing a block of size %s", au::Format::string(size).c_str() ));
                
                parser->run( data , size ,  writer );

            }
            
            parser->finish();
            
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
            
            
            map->init();
            
            std::set< Block* >::iterator b;
            for ( b = blocks.begin() ; b != blocks.end() ; b++)
            {
                KVSetStruct* inputStructs = (*b)->getKVSetStruct();

                LM_M(("Stream Mapping a block of size %s", au::Format::string( (*b)->getSize() ).c_str() ));
                
                //map->run( inputStructs , writer );
                
                delete inputStructs;
                
            }
            
            map->finish();
            
            // Detele the created instance
            delete map;
            
        }        
        
        
    }
}