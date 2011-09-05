

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
        
        if ( header.getFormat() == samson::KVFormat("txt","txt") )
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
        format = header.getFormat(); 
        
        // Get hash-group information
        if ( header.getFormat() == samson::KVFormat("txt","txt") )
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
            output << info[i].size << " ";
        
        return output.str();
    }
    
    void SamsonFile::printContent( size_t &limit )
    {
        if( error.isActivated())
            return;
        
        if ( header.getFormat() == samson::KVFormat("txt","txt") )
        {
            // txt content
            char buffer[1025];
            int n = fread(buffer, 1, 1024, file);
            
            while( (limit>0) && ( n > 0) )
            {
                write(1, buffer, n);
                n = fread(buffer, 1,1024, file);
                
                limit-= n;
            }
            
            return;
        }
        
        
        samson::ModulesManager* modulesManager = samson::ModulesManager::shared();
        samson::Data *keyData = modulesManager->getData(format.keyFormat);
        samson::Data *valueData = modulesManager->getData(format.valueFormat);
        
        if(!keyData )
        {
            std::cerr << "Data format " << format.keyFormat << " not supported\n";
            exit(0);
        }
        
        if(!valueData )
        {
            std::cerr << "Data format " << format.valueFormat << " not supported\n";
            exit(0);
        }
        
        
        
        samson::DataInstance *key = (samson::DataInstance *)keyData->getInstance();
        samson::DataInstance *value = (samson::DataInstance *)valueData->getInstance();
        
        for (int i = 0 ; i < KVFILE_NUM_HASHGROUPS ;i++)
        {
            if( limit <= 0 )
                return;
            limit--;
            
            size_t size = info[i].size;
            size_t kvs = info[i].kvs; 
            
            if( size > 0)
            {
                char *data = (char*)malloc( size );
                
                int n = fread(data, size, 1, file);
                if ( n != 1)
                {
                    std::cerr << "Not possible to read file. Wrong format\n";
                    exit(0);
                }
                
                
                size_t offset = 0 ;
                for (size_t i = 0 ; i < kvs ; i++)
                {
                    offset += key->parse(data+offset);
                    offset += value->parse(data+offset);
                    
                    std::cout << key->str() << " " << value->str() << std::endl;
                }
                
                if( offset != size)
                    LM_X(1 , ("Wrong file format"));
                
                free( data );
            }
            
            
        }        
    }
    
}
