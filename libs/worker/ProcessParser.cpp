

#include "ProcessParser.h"			// Own Interface
#include "WorkerSubTask.h"			// ss::ParserSubTask
#include "WorkerTask.h"				// ss::WorkerTask
#include "ModulesManager.h"			// ss::ModulesManager
#include "Packet.h"					// ss::BufferHeader

namespace ss
{

	ProcessParser::ProcessParser( ParserSubTask *_parserSubTask ) : ProcessBase( _parserSubTask->task , ProcessBase::key_value  )
	{
		parserSubTask = _parserSubTask;
		
		// Name of the generator
		operation_name = workerTask->operation();	
		
		// Set the buffer size the max size
        if( parserSubTask->buffer )
        {
            parserSubTask->buffer->setSize( parserSubTask->buffer->getMaxSize() );
            
            // Geting the operation and the data base address
            ModulesManager *modulesManager = ModulesManager::shared();
            operation = modulesManager->getOperation( operation_name );

            data = parserSubTask->buffer->getData();
            size = parserSubTask->buffer->getSize();		
        }
        else
        {
            data = NULL;
            size = 0;
        }
	}

	void ProcessParser::generateKeyValues( KVWriter *writer )
	{
        
        if( data )
        {
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
            

            // Non compressed fiels
            parser->init();
            parser->run( data , size ,  writer );
            parser->finish();
            
            // Detele the created instance
            delete parser;
        }

		/*
		 // Compressed files
		Buffer* b = Packet::decompressBuffer( parserSubTask->buffer );
		parser->init();
		parser->run( b->getData() , b->getSize() ,  writer );
		parser->finish();
		Engine::shared()->memoryManager.destroyBuffer(b);
		*/	
	}	

}
