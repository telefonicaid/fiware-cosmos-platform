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
#include <dirent.h>


#include "SamsonDataSet.h"      // Own interface
#include "samson/common/coding.h"


namespace samson {

    SamsonDataSet::SamsonDataSet( std::string directory )
    {
        struct stat filestatus;
        stat( directory.c_str() , &filestatus );
        
        if( !S_ISDIR( filestatus.st_mode ) )
        {
            error.set( au::str("%s is not a directory" , directory.c_str() ) );
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
                        {
                        	LM_E(("Error, file %s in directory %s reported error: %s" , pent->d_name , directory.c_str() , samsonFile->getErrorMessage().c_str()));
                            error.set( au::str("File %s in directory %s reported error: %s" , pent->d_name , directory.c_str() , samsonFile->getErrorMessage().c_str() ) );
                        }
                        
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
            if( !format.isEqual( format2 ) )
            {
            	LM_E(("Different formats found in included files, format1:'<%s-%s>' in file:'%s', format2:'<%s-%s>' in file:'%s'", format.keyFormat.c_str(), format.valueFormat.c_str(), files.begin()->first.c_str(), format2.keyFormat.c_str(), format2.valueFormat.c_str(), file->first.c_str()));
                error.set("Different formats found in included files");
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
    
    std::string SamsonDataSet::strSize()
    {
        return info.str();
    }
    
    std::string SamsonDataSet::strFormat()
    {
        return format.str();
    }
    
    void SamsonDataSet::printHashGroups(  std::ostream &output )
    {
        au::map< std::string , SamsonFile >::iterator file;
        for ( file = files.begin() ; file != files.end() ; file++)
            output << file->first << ": " << file->second->getHashGroups() << std::endl;
    }
    
    void SamsonDataSet::printHeaders(  std::ostream &output )
    {
        au::map< std::string , SamsonFile >::iterator file;
        for ( file = files.begin() ; file != files.end() ; file++)
            output << file->first << ": " << file->second->header.str() << std::endl;
    }
    
    
    void SamsonDataSet::printContent( size_t limit ,  std::ostream &output )
    {
        size_t records = 0;
        
        au::map< std::string , SamsonFile >::iterator file;
        for ( file = files.begin() ; file != files.end() ; file++)
        {
            records += file->second->printContent( (limit==0)?0:(limit - records ) , output );

			if( limit > 0 )
			   if( records >= limit )
				  return;
        }
        
    }
}
