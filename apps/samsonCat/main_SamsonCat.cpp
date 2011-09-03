
#include <iostream>
#include <sys/stat.h>					// stat(.)
#include <dirent.h>						// DIR directory header	

#include "au/CommandLine.h"			// au::CommandLine
#include "samson/common/coding.h"					// samson::FormatHeader
#include "samson/module/KVFormat.h"		// samson::KVFormat
#include "samson/module/ModulesManager.h"			// samson::ModulesManager
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "samson/common/SamsonSetup.h"			// samson::SamsonSetup

int logFd = -1;

//char* progName = (char*) "samsonCat";

class SamsonFile
{

    std::string fileName;
    au::ErrorManager error;

	FILE *file;
    
public:

    samson::KVHeader header;
    
    samson::KVFormat format;
    
    samson::KVInfo*  info;
    
    ~SamsonFile()
    {
        if( file )
            fclose( file );
        
        if( info )
            delete info;
    }
    
    SamsonFile( std::string _fileName )
    {
        fileName = _fileName;
        file = fopen( fileName.c_str() , "r" );        
        
        if( !file )
            error.set("Not possible to open file");
     
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
    
    bool hasError()
    {
        return ( error.isActivated() );
    }

    void printErrorAndExitIfAny()
    {
        if( error.isActivated() )
        {
            std::cerr << "SamsonFile ['" << fileName << "']: Error " << error.getMessage();
            exit(1);
        }
    }

    void printHashGroups()
    {
        if( !info)
            return;
        
        for (int i = 0 ; i < KVFILE_NUM_HASHGROUPS ; i++)
        {
            size_t s = info[i].size ;
            printf("%lu\n", s);
        }
    }
    
    void printContent( int limit )
    {
        if ( header.getFormat() == samson::KVFormat("txt","txt") )
        {
            // txt content
            char buffer[1025];
            int n = fread(buffer, 1, 1024, file);
            
            while( n > 0)
            {
                write(1, buffer, n);
                n = fread(buffer, 1,1024, file);
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
            if( (i>0) &&  (i == limit) )
                break;
            
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
    
};


int main(int argc, const char *argv[])
{
    
    
	au::CommandLine cmdLine;
	cmdLine.set_flag_boolean("header");
	cmdLine.set_flag_boolean("hg");
	cmdLine.set_flag_boolean("debug");
	cmdLine.set_flag_int("limit" , KVFILE_NUM_HASHGROUPS);
	cmdLine.set_flag_string("working",SAMSON_DEFAULT_WORKING_DIRECTORY);
	cmdLine.parse(argc , argv);

	samson::SamsonSetup::init(  );
    samson::SamsonSetup::shared()->setWorkingDirectory(cmdLine.get_flag_string("working"));
    samson::ModulesManager::init();
	
	int limit = cmdLine.get_flag_int("limit");
	
	
	if( cmdLine.get_num_arguments() < 2 ) 
	{
		std::cerr << "Usage: samsonCat file_or_directory [-header] [-hg]\n";
		exit(0);
	}

	std::string fileName = cmdLine.get_argument(1);

    
    struct stat filestatus;
    stat( fileName.c_str() , &filestatus );
    
    if ( S_ISREG( filestatus.st_mode ) )
    {
        
        // Open a single file
        SamsonFile samsonFile( fileName );
        samsonFile.printErrorAndExitIfAny();
        
        if( cmdLine.get_flag_bool("header") )
        {
            std::cout << samsonFile.header.str() << " " << " [ " << samsonFile.header.info.kvs << " kvs in " << samsonFile.header.info.size << "bytes\n";            
            exit(0);
        }
        
        if( cmdLine.get_flag_bool("hg" ) )
        {
            samsonFile.printHashGroups();
            exit(0);
        }
        
        samsonFile.printContent( limit );
        
    }
    else if( S_ISDIR( filestatus.st_mode ) )
    {
        
        // first off, we need to create a pointer to a directory
        DIR *pdir = opendir (fileName.c_str()); // "." will refer to the current directory
        struct dirent *pent = NULL;
        if (pdir != NULL) // if pdir wasn't initialised correctly
        {
            while ((pent = readdir (pdir))) // while there is still something in the directory to list
                if (pent != NULL)
                {
                    std::ostringstream localFileName;
                    localFileName << fileName << "/" << pent->d_name;
                    
                    struct stat buf2;
                    stat( localFileName.str().c_str() , &buf2 );
                    
                    if( S_ISREG(buf2.st_mode) )
                    {
                        SamsonFile samsonFile( localFileName.str() );
                        samsonFile.printErrorAndExitIfAny();
                        
                        if( cmdLine.get_flag_bool("header") )
                            std::cout << localFileName.str() << ": " << samsonFile.header.str() << std::endl;
                        else if( cmdLine.get_flag_bool("hg" ) )
                        {
                            std::cout << "File: " << localFileName;
                            samsonFile.printHashGroups();
                        }
                        else
                            samsonFile.printContent( limit );
                        
                    }
                    
                }
            // finally, let's close the directory
            closedir (pdir);						
        }
        
    } else
    {
        std::cerr << "Error:"  << fileName << " is not a file or a directory.\n";
    }
    
    
	
	
	
}
