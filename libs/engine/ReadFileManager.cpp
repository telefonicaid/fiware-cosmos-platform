
#include "ReadFileManager.h"        // Own interface
#include "ReadFile.h"               // engine::ReadFile

namespace engine {
    
    
    ReadFileManager::ReadFileManager()
    {
        // Default number of open files
        max_open_files = 100;
    }
    
    ReadFile *ReadFileManager::getReadFile( std::string fileName )
    {
        ReadFile *f = read_files.extractFromMap( fileName );
        
        // Remove non-valid ReadFiles
        if( f && !f->isValid() )
        {
            delete f;
            f = NULL;
        }
        
        if( !f )
            f = new ReadFile( fileName );
        
        // Insert at front ( remove the last used at the  back )
        read_files.insertAtFront( fileName , f );
        
        // Remove old File descriptors if necessary
        while( (int)read_files.size() > max_open_files )
        {
            
            ReadFile *rf = read_files.extractFromBack();
            
            if( rf == f )
                LM_X(1, ("Internal error"));
            
            rf->close();
            delete rf;
        }
        
        return f;
    }
    
}
