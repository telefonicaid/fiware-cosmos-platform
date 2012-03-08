

#include <sys/stat.h>

#include "samson/module/ModulesManager.h"           // samson::ModulesManager

#include "SamsonFile.h"                             // Own interface

namespace samson {
    
    SamsonFile::SamsonFile( std::string _fileName )
    {
        fileName = _fileName;
        file = fopen( fileName.c_str() , "r" );        
        info = NULL;
        
        if( !file )
        {
            error.set("Not possible to open file");
            return;
        }
        
        // Read header
        int nb = fread(&header, 1, sizeof(samson::KVHeader), file);
        
        if (nb != sizeof(samson::KVHeader))
            error.set(au::str("Getting header: read only %d bytes (wanted to read %ld)\n", nb, (long int) sizeof(samson::KVHeader)));
        
        if (!header.check())
        {
            error.set("Wrong magic number in header");
            return;
        }
        
        
        // Check file-size
        
        // Check the lenght of the file
        struct stat filestatus;
        stat( fileName.c_str() , &filestatus );
        
        if ( header.getKVFormat() == samson::KVFormat("txt","txt") )
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
                
                error.set( message.str() );
                
                std::cout << header.str() << "\n";
                
                return;
            }
            
        }
        else
        {
            size_t expected_size =   (size_t)( sizeof(samson::KVHeader) + (sizeof(samson::KVInfo)*KVFILE_NUM_HASHGROUPS) + header.info.size ) ;            
            
            if( (size_t)filestatus.st_size != expected_size)
            {
                std::ostringstream message;
                message << "Wrong file length\n";
                message << "Expected:";
                message << " Header: " << sizeof(samson::KVHeader);
                message << " + Info: " << (sizeof(samson::KVInfo)*KVFILE_NUM_HASHGROUPS);
                message << " + Data: " <<  header.info.size;
                message << " = " << expected_size << "\n";
                message << "File size: " << filestatus.st_size << " bytes\n";
                
                error.set( message.str() );
                return;
            }
            
        }
        
        
        
        // Get format
        format = header.getKVFormat(); 
        
        // Get hash-group information
        if ( header.getKVFormat() == samson::KVFormat("txt","txt") )
            info = NULL;
        else
        {
            
            info = (samson::KVInfo*) malloc(  sizeof(samson::KVInfo)*(KVFILE_NUM_HASHGROUPS));
            int          total_size =  sizeof(samson::KVInfo) * (KVFILE_NUM_HASHGROUPS);
            nb = fread(info, 1, total_size, file);
            
            if (nb != total_size)
            {
                error.set("Error reading information about hash-groups");
                return;
            }
            
        }
        
    }
    
    SamsonFile::~SamsonFile()
    {
        if( file )
            fclose( file );
        
        if( info )
            delete info;
    }
    
    
    
    bool SamsonFile::hasError()
    {
        return ( error.isActivated() );
    }
    
    std::string SamsonFile::getErrorMessage()
    {
        return error.getMessage();
    }
    
    
    std::string SamsonFile::getHashGroups()
    {
        if( !info)
            return "[No info]";
        
        std::ostringstream output;
        for (int i = 0 ; i < KVFILE_NUM_HASHGROUPS ; i++)
            output << i << " " << info[i].kvs << " " << info[i].size << "\n";
        
        return output.str();
    }
    
    size_t SamsonFile::printContent( size_t limit , std::ostream &output )
    {
        // Number of records displayed    
        size_t records = 0 ;
        
        if( error.isActivated() )
            return records;
        
        if ( header.getKVFormat() == samson::KVFormat("txt","txt") )
        {
            // txt content
            char buffer[1025];
            
            while( fgets(buffer, 1024, file) )
            {
                output << buffer;
                records++;

                if( limit > 0 )
                    if ( records >= limit )
                        return records;
            }
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
        
        for (int i = 0 ; i < KVFILE_NUM_HASHGROUPS ;i++)
        {
            
            size_t size = info[i].size;
            size_t kvs = info[i].kvs; 
            
            if( size > 0)
            {
                char *data = (char*)malloc( size );
                
                int n = fread(data, size, 1, file);
                if ( n != 1)
                {
                    output << "Not possible to read file. Wrong format\n";
                    return 0;
                }
                
                size_t offset = 0 ;
                for (size_t i = 0 ; i < kvs ; i++)
                {
                    offset += key->parse(data+offset);
                    offset += value->parse(data+offset);
                    
                    output << key->str() << " " << value->str() << std::endl;
                    
                    records++;
                    if( limit > 0)
                        if( records >= limit )
                            return records;
                    
                }
                
                if( offset != size)
                {
                    output << "Wrong file format\n";
                    return records;
                }
                
                free( data );
            }
            
            
        }
        
        return records;
        
    }
    
}
