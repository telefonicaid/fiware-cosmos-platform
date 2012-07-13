#ifndef _H_SamsonClientBlock_SamsonClient
#define _H_SamsonClientBlock_SamsonClient

#include "engine/BufferContainer.h"

namespace  samson 
{

    /*
     class used to be able to read a engine::Buffer with live data from a queue
     */
    
    
    class SamsonClientBlock : public SamsonClientBlockInterface
    {
        // Container of the in-memory buffer
        engine::BufferContainer buffer_container; 
        
        samson::KVHeader *header;          // Pointer to the header
        samson::KVFormat format;           // Format 
        
        au::ErrorManager error;            // Error message
        
        char *data;                        // Pointer to data content
        
    public:
        
        SamsonClientBlock( engine::Buffer * buffer )
        {
            buffer_container.setBuffer( buffer );

            // Header if always at the begining of the buffer
            header = (KVHeader*) buffer->getData();

            // Check header
            if( !header->check() )
            {
                error.set("Header check failed");
                return;
            }
            
            // Get the format from the header
            format =  header->getKVFormat();
            
            size_t expected_size;
            
            if ( format.isTxt() )
            {
                expected_size =   (size_t)( sizeof(samson::KVHeader)  + header->info.size );
                data = buffer->getData() + sizeof(samson::KVHeader);
            }
            else
            {
                expected_size =   (size_t)( sizeof(samson::KVHeader) + header->info.size ) ;           
                data = buffer->getData() + sizeof(samson::KVHeader);
            }
            
            if( expected_size != buffer->getSize() )
                error.set("Wrong file format");
            
            // Check if we have data types installed here
            samson::ModulesManager* modulesManager = samson::ModulesManager::shared();
            samson::Data *keyData = modulesManager->getData(format.keyFormat);
            samson::Data *valueData = modulesManager->getData(format.valueFormat);

            if( !format.isTxt() )
            {
                if( !keyData )
                    error.set( au::str("Unknown data type %s" , format.keyFormat.c_str() ) );
                
                if( !valueData )
                    error.set( au::str("Unknown data type %s" , format.valueFormat.c_str() ) );
            }
            
        }
        
        ~SamsonClientBlock()
        {
        }
        
        size_t getBufferSize()
        {
            return buffer_container.getBuffer()->getSize();
        }
        
        size_t getTXTBufferSize()
        {
            return buffer_container.getBuffer()->getSize() - sizeof( samson::KVHeader );
        }
        
        char* getTXTBufferPointer()
        {
            return data;
        }
        
        
        KVHeader getHeader()
        {
            return *header;
        }
        

        std::string get_header_content()
        {
            if( error.isActivated() )
                return au::str( "ERROR: %s\n" , error.getMessage().c_str() );
            return header->str() + "\n";
        }
        
        std::string get_content(int max_kvs, const char * outputFormat="plain")
        {
            std::ostringstream output;
            
            if( error.isActivated())
            {
                output << "ERROR: " << error.getMessage() << "\n";
                output.str();
            }
            
            if ( format.isTxt() )
            {
                // Search for the position to plot...
                size_t pos = header->info.size;
                if( max_kvs > 0 )
                {
                    pos = 0;
                    int lines = 0;
                    while (pos < header->info.size) 
                    {
                        if( data[pos] == '\n' )
                        {
                            lines++;
                            if( lines == max_kvs )
                                break;
                        }
                        pos++;
                    }
                }
                
				output.write( data, header->info.size);
                return output.str();
            }
            
            // Key value format
            samson::ModulesManager* modulesManager = samson::ModulesManager::shared();
            samson::Data *keyData = modulesManager->getData(format.keyFormat);
            samson::Data *valueData = modulesManager->getData(format.valueFormat);
            
            if(!keyData )
            {
                error.set( au::str("Data format %s not supported" , format.keyFormat.c_str() ) );
                output << "ERROR: " << error.getMessage() << "\n";
                return output.str();
            }
            
            if(!valueData )
            {
                error.set( au::str("Data format %s not supported" , format.valueFormat.c_str() ) );
                output << "ERROR: " << error.getMessage() << "\n";
                return output.str();
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
                
                if (strcmp(outputFormat, "plain") == 0)
                {
                    output << "[" << time_string << "] " << key->str() << " " << value->str() << std::endl;
                }
                else if (strcmp(outputFormat, "json") == 0)
                {
                    output << "[" << time_string << "] " << "{\"key\":" << key->strJSON() << ", \"value\":" << value->strJSON() << "}" << std::endl;
                }
                else if (strcmp(outputFormat, "xml") == 0)
                {
                    output << "[" << time_string << "] " << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?><key-value>\n<key>" << key->strXML() << "</key>\n<value>" << value->strXML() << "</value>\n</key-value>" << std::endl;
                }
                else
                {
                    output << "ERROR. outputFormat:'" << outputFormat << "' not supported" << std::endl;
                }
                
                num_kvs++;
                if( max_kvs > 0 )   
                    if( num_kvs >= (size_t) max_kvs )
                    {
                        size_t rest_kvs = header->info.kvs - max_kvs;
                        if( rest_kvs > 0 )
                            output << au::str( "... %s more key-values\n", au::str( rest_kvs ).c_str() , rest_kvs );
                        
                        return output.str();
                    }
                
            }
            
            return output.str();
            
        }
        
    };
    
}

#endif
