#include <fcntl.h>
#include <dirent.h>
#include "au/ThreadManager.h"
#include "StreamConnector.h"
#include "DiskAdaptor.h" // Own interface

namespace stream_connector {
    
    void* run_DiskConnection(void*p)
    {
        DiskConnection* d = (DiskConnection*)p;
        d->run();
        d->thread_running = false; // notify that thread is not running any more
        return NULL;
    }
    
    std::string nameDiskConnection( ConnectionType type , std::string fileName )
    {
        switch (type) {
            case connection_input:
                return au::str("Reading from %s" , fileName.c_str() );
                break;
            case connection_output:
                return au::str("Writing to %s" , fileName.c_str() );
                break;
        }
        
        return "Error";
    }
    
    DiskConnection::DiskConnection( Adaptor  * _item 
                                   , ConnectionType _type 
                                   , std::string _file_name ) 
    : Connection( _item , _type , nameDiskConnection( _type ,  _file_name ) )
    , token("DiskConnection")
    {
        // Keep  copy of the file name
        file_name = _file_name;
        
        // Default input buffer size
        input_buffer_size = 1024;
        
        // Open files if input
        if( getType() == connection_input )
        {
            // Analyse files to be read...
            struct stat s;
            stat( file_name.c_str() , &s);
            
            if( S_ISDIR( s.st_mode ) )
            {
                // Add all plain files in this directory
                DIR *pdir = opendir (file_name.c_str()); // "." will refer to the current directory
                struct dirent *pent = NULL;
                if (pdir != NULL) // if pdir wasn't initialised correctly
                {
                    while ((pent = readdir (pdir))) // while there is still something in the directory to list
                        if (pent != NULL)
                        {                    
                            if( strcmp( "." , pent->d_name )==0 )
                                continue;
                            if( strcmp( ".." , pent->d_name )==0 )
                                continue;
                            
                            std::ostringstream localFileName;
                            localFileName << file_name << "/" << pent->d_name;
                            
                            // Add to the list of files
                            files.push_back( localFileName.str() );
                            
                        }
                    // finally, let's close the directory
                    closedir (pdir);						
                }
            }
            else if( S_ISREG( s.st_mode ) )
                files.push_back( file_name );
        }
        else
        {
            // Create directory...
            int s = mkdir( file_name.c_str() , 0755 );
            if( ( s != 0 ) && ( errno != EEXIST) )
            {
                error.set(  au::str("Problems creating directory %s (%s)" , file_name.c_str() , strerror(errno) ) );
            }
            
        }
        
        // No file descriptor by default
        file_descriptor = NULL;
        
        // Thread to executed by default
        thread_running = false;
    }
    
    DiskConnection::~DiskConnection()
    {
    }
    
    void DiskConnection::start_connection()
    {
        if ( thread_running )
            return;
        
        // Flag to indidicate the background thread should return ASAP
        stoping_threads = false;
        
        // Create the background thread
        thread_running = true;
        pthread_t t;
        au::ThreadManager::shared()->addThread("DiskConnection",&t, NULL, run_DiskConnection, this );
    }
    
    void DiskConnection::stop_connection()
    {
        // Create the background thread
        stoping_threads = true;
        
        while( thread_running )
            usleep(100000);
    }
    
    
    void DiskConnection::run_as_output()
    {
        std::string current_file_name;
        int file_counter = 0;
        size_t accumulated_size = 0;
        size_t max_size = 64000000; // Should be a parameter
        
        while( true )
        {
            if( stoping_threads )
                return;
            
            // Get the next buffer to be transmitted
            engine::BufferContainer container;
            getNextBufferToSent(&container);
            
            // Get a pointer to this buffer
            engine::Buffer* current_buffer = container.getBuffer();
            
            if( !current_buffer )
            {
                usleep(100000);
                continue;
            }
            
            // Size of the block
            size_t current_size = current_buffer->getSize();
            
            if( current_size> max_size )
            {
                error.set( au::str("Received a block with size %s. Max file size %s." 
                                   , au::str( current_size,"B" ).c_str() 
                                   , au::str( max_size ,"B" ).c_str() ) );
                return;
            }
            
            if( ( accumulated_size + current_size ) > max_size )
            {
                file_descriptor->close();
                delete file_descriptor;
                file_descriptor = NULL;
            }
            
            
            if( !file_descriptor )
            {
                accumulated_size = 0;
                
                while( true )
                {
                    current_file_name = au::str("%s/file_%d" , file_name.c_str() , file_counter );
                    
                    struct stat buf;
                    int s = stat( current_file_name.c_str() , &buf );
                    if( s == 0 )
                    {
                        file_counter++;
                        continue; // found
                    }
                    
                    // Open a file to write
                    int _fd = open( current_file_name.c_str() , O_CREAT | O_WRONLY , 0644 );
                    
                    if( _fd < 0 )
                    {
                        error.set( au::str("Not possible to open file %s (%s)" , current_file_name.c_str() , strerror(errno) ) );
                        return;
                    }
                    
                    file_descriptor = new au::FileDescriptor( current_file_name , _fd  );
                    break;
                }
                
            }
            
            // Write to disk
            au::Status s = file_descriptor->partWrite(current_buffer->getData(), current_buffer->getSize(), "samsonConnectorConnection");
            
            if( s != au::OK )
            {
                
                error.set( au::str("Not possible to write buffer with %s to file %s" 
                                   , au::str( current_buffer->getSize() ).c_str()
                                   , current_file_name.c_str() ) );
                return;
            }
            
            accumulated_size += current_size;
            
        }
        
        
    }
    
    void DiskConnection::run_as_input()
    {
        
        while( true )
        {
            
            if( stoping_threads )
                return;
            
            if( file_descriptor )
            {
                // Still reading from a file...
                engine::Buffer * buffer = engine::MemoryManager::shared()->createBuffer("stdin"
                                                                                        , "connector"
                                                                                        , input_buffer_size );
                
                // Read the entire buffer
                size_t read_size = 0;
                au::Status s;
                {
                    au::Cronometer c;
                    s = file_descriptor->partRead(buffer->getData()
                                                  , input_buffer_size
                                                  , "read connector connections"
                                                  , 300 
                                                  , &read_size );
                    
                    if( c.diffTime() < 0.1 )
                        input_buffer_size *= 2;
                    else if( c.diffTime() > 3 )
                        input_buffer_size /= 2;
                    
                }
                // If we have read something...
                if( read_size > 0 )
                {
                    // Set the buffer size
                    buffer->setSize(read_size);
                    pushInputBuffer(buffer);
                }
                
                // Release alocated buffer
                buffer->release();
                
                // If last read is not ok...
                if( s != au::OK )
                {
                    // Close current file descriptor
                    delete file_descriptor;
                    file_descriptor = NULL;
                }                  
            }
            else
            {
                if( files.size() == 0)
                    return;  // Nothing else to be read
                
                // Open the next file...
                std::string current_file_name = files.front();
                files.pop_front();
                
                int _fd = open( current_file_name.c_str() , O_RDONLY );
                
                if( _fd > 0 )
                    file_descriptor = new au::FileDescriptor( file_name , _fd );
                else
                {
                    // Skipping file, show a message?
                }
                
            }
            
            
        }
        
    }
    
    void DiskConnection::run()
    {
        if( error.isActivated() )
            return;
        
        if( getType() == connection_input )
            run_as_input();
        else
            run_as_output();
    }
    
    std::string DiskConnection::getStatus()
    {
        au::TokenTaker tt(&token);
        std::ostringstream output;
        
        
        if ( thread_running )
            output << "Running";
        else
            output << "Stoped";
        
        if( error.isActivated() )
            output << au::str(" [Error:%s]" , error.getMessage().c_str() );
        
        
        if( getType() == connection_input) 
            output << au::str( " [Buffer:%s]" , au::str(input_buffer_size).c_str() );
        
        return output.str();
    }
    
    void DiskConnection::review_connection()
    {
        if( file_descriptor )
            set_as_connected( !file_descriptor->isDisconnected() );
        else
            set_as_connected(true);
    }
    
    
    DiskAdaptor::DiskAdaptor( Channel * _channel , ConnectionType _type ,  std::string _directory ) : 
    Adaptor( _channel , _type , au::str("FILE(%s)" , _directory.c_str() ) ) 
    {
        // Information for connection
        directory = _directory;
        
        // Add internal connection just once...
        // Add connection
        add( new DiskConnection( this , getType() , directory ) );
        
    }
    
    // Get status of this element
    std::string DiskAdaptor::getStatus()
    {
        return "Disk connection";
    }

}
