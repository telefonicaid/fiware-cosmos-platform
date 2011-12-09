
#include "ReadFileManager.h"        // Own interface
#include "ReadFile.h"               // engine::ReadFile

#include "logMsg/traceLevels.h"            // LmtIsolated, etc.

NAMESPACE_BEGIN(engine)

ReadFileManager::ReadFileManager()
{
    // Default number of open files
    max_open_files = 100;
}

ReadFileManager::~ReadFileManager()
{
    read_files.clearListMap();
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
        
        LM_T(LmtIsolated, ("For file:'%s'(%d), Maximum number of opened files reached: read_files.size()(%d) > max_open_files:(%d), closing rf:%d", fileName.c_str(), fileno(f->file), read_files.size(), max_open_files, fileno(rf->file)));
        
        if( rf == f )
            LM_X(1, ("Internal error closing extra descriptors when trying to read '%s': rf(%d) == f(%d)", fileName.c_str(), fileno(rf->file), fileno(f->file)));
        
        rf->close();
        delete rf;
    }
    
    return f;
}


NAMESPACE_END