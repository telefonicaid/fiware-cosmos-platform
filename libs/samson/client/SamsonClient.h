
/* ****************************************************************************
 *
 * FILE            SamsonClient.h
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         Samson
 *
 * DATE            7/14/11
 *
 * DESCRIPTION
 *
 *
 * COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/

#ifndef _H_Samson_SamsonClient
#define _H_Samson_SamsonClient

#include <string>       // std::string
#include <vector>       // std::vector
#include <pthread.h>        /* pthread*/


#include "au/Cronometer.h"          // au::Cro
#include "au/Token.h"               // au::Token
#include "au/TokenTaker.h"          // au::TokenTaker
#include "au/list.h"               // au::list
#include "au/map.h"               // au::map
#include "au/Rate.h"               // au::rate::Rate

#include "engine/Object.h"          // engine::Object
#include "engine/Buffer.h"          // engine::Buffer
#include "engine/MemoryManager.h"   // engine::MemoryManager

#include "samson/common/coding.h"

#include "samson/module/ModulesManager.h"

namespace  samson {
    
    class SamsonClient;
    
    
    /*
     Class used to push data to a txt queue in buffer mode
     */
     
    class SamsonPushBuffer
    {
        
        SamsonClient *client;
        std::string queue;

        // Buffer
        char *buffer;
        size_t max_buffer_size;
        
        size_t size;    // Current size of the buffer
        
        au::Token token;
    
    public:
        
        au::rate::Rate rate; // Statistics about rate
        
        SamsonPushBuffer( SamsonClient *client , std::string queue );
        ~SamsonPushBuffer();

        void push( const char *data , size_t length , bool flushing );
        void flush();

        // Recevie notifications
        void notify( engine::Notification* notification );
        
    private:
        
        void _flush();      // No token protected flush operation

        
    };
    
    
    class BufferList
    {
        au::list< engine::Buffer > buffers;
        
    public:

        void push( engine::Buffer* buffer )
        {
            if( !buffer )
                return;
            
            buffers.push_back( buffer );
        }
        
        engine::Buffer *pop()
        {
            return buffers.extractFront();
        }
        
    };
    

    class BufferContainer 
    {
        au::Token token;
        au::map< std::string , BufferList > buffer_lists;    // Buffers for live data
        
    public:
        
        BufferContainer( ) : token("BufferContainer")
        {
            // listen...
        }
        
        void push( std::string queue , engine::Buffer* buffer )
        {
            au::TokenTaker tt(&token);
            getBufferList( queue )->push( buffer );
        }
        
        engine::Buffer* pop( std::string queue )
        {
            au::TokenTaker tt(&token);
            return getBufferList(queue)->pop();
        }
        
        

    private:
        
        BufferList* getBufferList( std::string name )
        {
            return buffer_lists.findOrCreate(name);
        }

        
    };
    
    // Class to work with blocks

    class SamsonClientBlockInterface
    {
        
    public:

        virtual size_t getBufferSize()=0;
        
        virtual size_t getTXTBufferSize()=0;
        virtual char* getTXTBufferPointer()=0;
        
        virtual void print_header()=0;
        virtual void print_content(int max_kvs)=0;
        
    };

    
    class SamsonClientBlock : public SamsonClientBlockInterface
    {
        engine::Buffer *buffer;
        samson::KVHeader *header;
        samson::KVFormat format;        // Format 

        KVInfo *info;                   // Info vector

        au::ErrorManager error;         // Error message
        
        char *data;                     // Pointer to data content
        
    public:
        
        SamsonClientBlock( engine::Buffer *_buffer )
        {
            
            buffer = _buffer;
            header = (KVHeader*) buffer->getData();
            
            if( !header->check() )
            {
                error.set("Header check failed");
                info = NULL;
                return;
            }
            
            // Get the format from the header
            format =  header->getKVFormat();
            
            size_t expected_size;
            
            if ( format.isTxt() )
            {
                expected_size =   (size_t)( sizeof(samson::KVHeader)  + header->info.size );
                info = NULL;
                data = buffer->getData() + sizeof(samson::KVHeader);
            }
            else
            {
                expected_size =   (size_t)( sizeof(samson::KVHeader) + (sizeof(samson::KVInfo)*KVFILE_NUM_HASHGROUPS) + header->info.size ) ;           
                info = (KVInfo*) buffer->getData() + sizeof(samson::KVHeader);
                data = buffer->getData() + sizeof(samson::KVHeader) + ( sizeof(samson::KVInfo)*KVFILE_NUM_HASHGROUPS );
            }
            
            if( expected_size != buffer->getSize() )
                error.set("Wrong size");
            
        }
        
        ~SamsonClientBlock()
        {
            engine::MemoryManager::shared()->destroyBuffer(buffer);
        }
        
        size_t getBufferSize()
        {
            return buffer->getSize();
        }

        size_t getTXTBufferSize()
        {
            return buffer->getSize() - sizeof( samson::KVHeader );
        }

        char* getTXTBufferPointer()
        {
            return data;
        }
        
        void print_header()
        {
            if( error.isActivated() )
            {
                std::cout << "ERROR: " << error.getMessage() << "\n";
                return;
            }
            std::cout << header->str() << "\n";
        }
        
        void print_content(int max_kvs)
        {
            if( error.isActivated())
            {
                std::cout << "ERROR: " << error.getMessage() << "\n";
                return;
            }
            
            if ( format.isTxt() )
            {
				volatile int nb;

				// Write the content to the output
				nb = write(1, data, header->info.size);
				return;
            }
            
            // Key value format
            samson::ModulesManager* modulesManager = samson::ModulesManager::shared();
            samson::Data *keyData = modulesManager->getData(format.keyFormat);
            samson::Data *valueData = modulesManager->getData(format.valueFormat);
            
            if(!keyData )
            {
                error.set( au::str("Data format %s not supported" , format.keyFormat.c_str() ) );
                std::cout << "ERROR: " << error.getMessage() << "\n";
                return;
            }
            
            if(!valueData )
            {
                error.set( au::str("Data format %s not supported" , format.valueFormat.c_str() ) );
                std::cout << "ERROR: " << error.getMessage() << "\n";
                return;
            }
            
            samson::DataInstance *key = (samson::DataInstance *)keyData->getInstance();
            samson::DataInstance *value = (samson::DataInstance *)valueData->getInstance();
            
            size_t offset = 0 ;
            size_t num_kvs = 0;
            for (size_t i = 0 ; i < header->info.kvs ; i++)
            {
                offset += key->parse(data+offset);
                offset += value->parse(data+offset);
                
                
                time_t _time = time(NULL);
                
                char time_string[1024];
                ctime_r(&_time, time_string );
                time_string[strlen(time_string) - 1] = '\0';
                
                std::cout << "[" << time_string << "] " << key->str() << " " << value->str() << std::endl;
                
                num_kvs++;
                if( max_kvs > 0 )   
                    if( num_kvs >= (size_t) max_kvs )
                    {
                        size_t rest_kvs = header->info.kvs - max_kvs;
                        if( rest_kvs > 0 )
                            std::cout << au::str( "Received %s more key-values\n", au::str( rest_kvs ).c_str() , rest_kvs );
                        
                        return;
                    }
                
            }        
        }
        
    };
    
    
    /*
        Main class to connect to a samson cluster
     */
    
    class SamsonClient
    {
            
        size_t memory;                          // Memory used internally for load / download operations
        
        size_t load_buffer_size;                // Size of the load buffer
        
        std::string error_message;              // Error message if a particular operation fails
        
        std::vector<size_t> delilah_ids;        // Delilah operation to wait for...
        
	public:

        au::rate::Rate rate;                    // Statistics about rate
        
        // Default constructor
        SamsonClient();

        // Set memory ( only useful before init )
        void setMemory ( size_t _memory );
        
        // Init the connection with a SAMSON cluster
        bool init( std::string controller );
        
        // Push content to a particular queue
        size_t push( std::string queue , char *data , size_t length );
        
        // Get error message
        std::string getErrorMessage();
        
        // Wait until all operations are finished
        void waitUntilFinish();        
        
        // Live data connection
        void connect_to_queue( std::string queue, bool flag_new , bool flag_remove );
        SamsonClientBlockInterface* getNextBlock( std::string queue );
        
        // Getting information about my connection
        std::string getInfoAboutPushConnections( bool print_verbose );
        std::string getInfoAboutDelilahComponents();
      
        std::string getStatisticsString()
        {
            return au::str("Pushed %s in %d blocs", au::str(rate.getTotalSize(),"B").c_str() , rate.getTotalNumberOfHits() );
        }
        
    };
    
}
#endif
