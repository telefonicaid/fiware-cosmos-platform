

#include "ReadFile.h"       // Own interface
#include "logMsg.h"         // LM_M ...

namespace engine {
    
    ReadFile::ReadFile( std::string _fileName )
    {
        fileName = _fileName;
        file = fopen( fileName.c_str() , "r" );
    }
    
    int ReadFile::seek( size_t offset )
    {
        if( !file )
            return 1;
        
        // Get the current offset
        off_t current_offset = ftell( file );
        if( current_offset == -1 )
        {
            LM_W(("ftell failed for file %s" , fileName.c_str() ));
            fclose( file );
            file = NULL;
            
            return 2;
        }

        // get a warning to be aware of this seeks if it is too large
        if( abs( (size_t)current_offset - offset ) > 100000  ) 
            LM_W(("Seeking file %s from %lu to %lu" , fileName.c_str() , current_offset , offset));
           
        
        if( fseek(file, offset, SEEK_SET) != 0 )
        {
            fclose( file );
            file = NULL;
            return 3;
        }
        
        // Everything corrent
        return 0;
        
    }
    
    int ReadFile::read( char * read_buffer , size_t size )
    {
        if ( size == 0)
            return 0;
        
        if ( fread( read_buffer, size, 1, file ) == 1 )
            return 0;
        else 
            return 1;   // Error at reading
        
    }
    
    bool ReadFile::isValid()
    {
        return (file!=NULL);
    }
    
    void ReadFile::close()
    {
        if( file )
        {
            fclose( file );
            file = NULL;
        }
    }
    
}