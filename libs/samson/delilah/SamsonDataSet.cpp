
#include <sys/stat.h>
#include <dirent.h>


#include "SamsonDataSet.h"      // Own interface


namespace samson {

    SamsonDataSet::SamsonDataSet( std::string directory )
    {
        struct stat filestatus;
        stat( directory.c_str() , &filestatus );
        
        if( !S_ISDIR( filestatus.st_mode ) )
        {
            error.set( au::str("%s is not a directory") );
            return;
        }
        
        // Loop inside the directory to scan
        DIR *pdir = opendir (directory.c_str()); 
        struct dirent *pent = NULL;
        if (pdir != NULL) // if pdir wasn't initialised correctly
        {
            while ((pent = readdir (pdir))) // while there is still something in the directory to list
                if (pent != NULL)
                {
                    std::ostringstream localFileName;
                    localFileName << directory << "/" << pent->d_name;
                    
                    struct stat buf2;
                    stat( localFileName.str().c_str() , &buf2 );
                    
                    if( S_ISREG(buf2.st_mode) )
                    {
                        SamsonFile *samsonFile = new SamsonFile( localFileName.str() );
                        
                        if( samsonFile->hasError() )
                            error.set( au::str("File %s in directory %s reported error: %s" , pent->d_name , directory.c_str() , samsonFile->getErrorMessage().c_str() ) );
                        
                        std::string name = pent->d_name;
                        files.insertInMap(name, samsonFile);
                        
                    }
                    
                }
            // finally, let's close the directory
            closedir (pdir);        
        }
        
        // Check common formats...
        if( files.size() == 0)
        {
            error.set("No content");
            return;
        }
        
        // Get common format
        format = files.begin()->second->format;
        
        au::map< std::string , SamsonFile >::iterator file;
        for ( file = files.begin() ; file != files.end() ; file++)
        {
            KVFormat format2 = file->second->format;
            if( format != format2 )
            {
                error.set("Different formats in included files");
                return;
            }
            
            KVInfo _info = file->second->header.info;
            info.append( _info );
            
        }
        
    }

    SamsonDataSet::~SamsonDataSet()
    {
        files.clearMap();
    }
    
    std::string SamsonDataSet::str()
    {
        std::ostringstream output;
        output << info.str() << " " << format.str();
        return output.str();
    }
    
    
    void SamsonDataSet::printHashGroups()
    {
        au::map< std::string , SamsonFile >::iterator file;
        for ( file = files.begin() ; file != files.end() ; file++)
            std::cout << file->first << ": " << file->second->getHashGroups() << std::endl;
    }
    
    void SamsonDataSet::printHeaders()
    {
        au::map< std::string , SamsonFile >::iterator file;
        for ( file = files.begin() ; file != files.end() ; file++)
            std::cout << file->first << ": " << file->second->header.str() << std::endl;
    }
    
    
    void SamsonDataSet::printContent( size_t limit )
    {
        au::map< std::string , SamsonFile >::iterator file;
        for ( file = files.begin() ; file != files.end() ; file++)
            file->second->printContent( limit );
        
    }
}