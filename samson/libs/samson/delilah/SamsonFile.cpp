/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */


#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>  // read

#include "samson/module/ModulesManager.h"           // samson::ModulesManager

#include "logMsg/logMsg.h"                                              // LM_T
#include "logMsg/traceLevels.h"            // LmtFileDescriptors, etc.

#include "SamsonFile.h"                             // Own interface

namespace samson {
    
    SamsonFile::SamsonFile( std::string _fileName )
    {
        fileName = _fileName;
        int fd = open( fileName.c_str() , O_RDONLY );
        LM_T(LmtFileDescriptors, ("Open FileDescriptor fd:%d", fd));
        
        if( fd < 0 )
        {
            error.set("Not possible to open file");

            return;
        }
        
        // Read header
        ssize_t nb = read(fd, &header, sizeof(samson::KVHeader));
        
        if (nb != sizeof(samson::KVHeader))
        {
        	LM_E(("Getting header: read only %d bytes (wanted to read %ld)\n", nb, (long int) sizeof(samson::KVHeader)));
            error.set(au::str("Getting header: read only %d bytes (wanted to read %ld)\n", nb, (long int) sizeof(samson::KVHeader)));
            close(fd);
            LM_T(LmtFileDescriptors, ("Closing SamsonFile because error fd:%d", fd));
            fd = -1;
            return;
        }
        
        if (!header.check())
        {
        	LM_E(("Wrong magic number in header"));
            error.set("Wrong magic number in header");
            close(fd);
            LM_T(LmtFileDescriptors, ("Closing SamsonFile because error fd:%d", fd));
            fd = -1;
            return;
        }
        
        
        // Check file-size
        
        // Check the lenght of the file
        struct stat filestatus;
        stat( fileName.c_str() , &filestatus );
        
        if ( header.getKVFormat().isTxt() )
        {
            size_t expected_size =   (size_t)( sizeof(samson::KVHeader)  + header.info.size );
            
            if( (size_t)filestatus.st_size != expected_size)
            {
                std::ostringstream message;
                message << "Wrong file length\n";
                message << "Expected:";
                message << " Header: " << sizeof(samson::KVHeader);
                message << " + Data: " <<  header.info.size;
                message << " = " << expected_size << "\n";
                message << "File size: " << filestatus.st_size << " bytes\n";
                
                LM_E(("Error: '%s'", message.str().c_str()));
                error.set( message.str() );
                
                std::cout << header.str() << "\n";
                LM_T(LmtFileDescriptors, ("Closing SamsonFile because error fd:%d", fd));
                close(fd);
                fd = -1;
                return;
            }
        }
        else
        {
            size_t expected_size =   (size_t)( sizeof(samson::KVHeader) + header.info.size ) ;            
            
            if( (size_t)filestatus.st_size != expected_size)
            {
                std::ostringstream message;
                message << "Wrong file length\n";
                message << "Expected:";
                message << " Header: " << sizeof(samson::KVHeader);
                message << " + Data: " <<  header.info.size;
                message << " = " << expected_size << "\n";
                message << "File size: " << filestatus.st_size << " bytes\n";
                
                error.set( message.str() );
                LM_T(LmtFileDescriptors, ("Closing SamsonFile because error fd:%d", fd));
                close(fd);
                fd = -1;
                return;
            }
        }
        
        // Get format
        format = header.getKVFormat(); 

        LM_T(LmtFileDescriptors, ("Closing SamsonFile fd:%d", fd));
        close(fd);
        fd = -1;
        return;
    }
    
    SamsonFile::~SamsonFile()
    {
        if( fd >= 0 )
        {
            LM_T(LmtFileDescriptors, ("Closing SamsonFile fd:%d", fd));
            close( fd );
        }
        fd = -1;
    }
    
    bool SamsonFile::hasError()
    {
        return ( error.isActivated() );
    }
    
    std::string SamsonFile::getErrorMessage()
    {
        return error.getMessage();
    }
    
    class SimpleBuffer
    {
        
    public:
        
        char* data;
        
        SimpleBuffer( std::string fileName )
        {
            size_t nb;

            // In case of error
            data = NULL;
            
            struct stat filestatus;
            if( stat( fileName.c_str() , &filestatus ) != 0 )
                return;            
            
            FILE *file = fopen( fileName.c_str() , "r" );    
            
            data = (char*) malloc(filestatus.st_size);
            
            nb = fread(data, filestatus.st_size, 1, file);
            if (nb == 0)
            {
            	LM_W(("No data read from file:'%s'", fileName.c_str()));
            }

            fclose(file);
        }
        
        SimpleBuffer( size_t size )
        {
            if( data )
                data = ( char* ) malloc( size );
        }
        
        ~SimpleBuffer()
        {
            free( data );
        }
        
    };

    
    std::string SamsonFile::getHashGroups()
    {
        SimpleBuffer simple_buffer( fileName );
        
        // If fileName could not be read, we must check simple_buffer.data
        if (simple_buffer.data == NULL)
        {
            LM_E(("Error, no data for fileName:'%s'", fileName.c_str()));
            return au::str("Error, no data for fileName:'%s'", fileName.c_str());
        }

        au::ErrorManager error;
        KVInfo* info = createKVInfoVector( simple_buffer.data , &error );

        if( !info)
            return au::str( "Error getting vector for hashgroups. (%s)" , error.getMessage().c_str() );;
        
        std::ostringstream output;
        for (int i = 0 ; i < KVFILE_NUM_HASHGROUPS ; i++)
            output << i << " " << info[i].kvs << " " << info[i].size << "\n";

        free( info );
        
        return output.str();
    }
    
    
    
    size_t SamsonFile::printContent( size_t limit , std::ostream &output )
    {
        // Number of records displayed    
        size_t records = 0 ;
        
        if( error.isActivated() )
            return records;
        
        if ( header.getKVFormat().isTxt() )
        {
            // txt content
            char buffer[1025];
            FILE *file = fopen( fileName.c_str() , "r" );

            if (file == NULL)
            {
            	LM_E(("Error opening file:'%s'", fileName.c_str()));
            	return 0;
            }

            
            while( fgets(buffer, 1024, file) )
            {
                output << buffer;
                records++;

                if( limit > 0 )
                {
                    if ( records >= limit )
                    {
                    	fclose(file);
                        return records;
                    }
                }
            }
            fclose(file);
            return records;
        }
        
        samson::ModulesManager* modulesManager = samson::ModulesManager::shared();
        samson::Data *keyData = modulesManager->getData(format.keyFormat);
        samson::Data *valueData = modulesManager->getData(format.valueFormat);
        
        if(!keyData )
        {
            output << "Data format " << format.keyFormat << " not supported\n";
            return records;
        }
        
        if(!valueData )
        {
            output << "Data format " << format.valueFormat << " not supported\n";
            return records;
        }
        
        samson::DataInstance *key = (samson::DataInstance *)keyData->getInstance();
        samson::DataInstance *value = (samson::DataInstance *)valueData->getInstance();
        
        
        // Load file in memory..
        SimpleBuffer file_buffer( fileName );
        char* data = file_buffer.data;
        
        size_t offset = sizeof(KVHeader);
        for ( int i = 0 ; i < (int)header.info.kvs ; i++ )
        {
            size_t key_size = key->parse(data+offset);
            offset += key_size;
            
            size_t value_size = value->parse(data+offset);
            offset += value_size;

            output << key->str() << " " << value->str() << std::endl;
            
            records++;
            if( limit > 0)
                if( records >= limit )
                    return records;
        }
        
        return records;
        
    }
    
}
