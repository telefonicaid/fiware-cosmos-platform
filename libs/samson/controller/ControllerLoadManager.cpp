

#include "ControllerLoadManager.h"      // Own interface


namespace samson
{
    
    /**
     Information about upload and download operations
     */
    
    ControllerLoadOperation::ControllerLoadOperation( size_t _task_id )
    {
        task_id = _task_id;
    }
    
    ControllerUploadOperation::ControllerUploadOperation(  size_t task_id ) : ControllerLoadOperation( task_id )
    {
    }
    
    void ControllerUploadOperation::getInfo( std::ostringstream& output)
    {
        au::xml_open( output, "updaload_operation");
        au::xml_simple( output , "task_id" , task_id );
        au::xml_close( output, "updaload_operation");
    }
    
    ControllerDownloadOperation::ControllerDownloadOperation( size_t task_id ) : ControllerLoadOperation( task_id )
    {
    }
    
    void ControllerDownloadOperation::getInfo( std::ostringstream& output)
    {
        au::xml_open( output, "downdaload_operation");
        au::xml_simple( output , "task_id" , task_id );
        au::xml_close( output, "downupdaload_operation");
    }
    
    ControllerLoadManager::ControllerLoadManager() : token( "ControllerLoadManager" )
    {
    }
    
    void ControllerLoadManager::getInfo( std::ostringstream& output)
    {
        au::xml_open( output, "load_manager");
        
        au::xml_iterate_map(output, "uploads", uploads );
        au::xml_iterate_map(output, "downloads", downloads );
        
        au::xml_close( output, "load_manager");
    }
    
    
    void ControllerLoadManager::addUpload( size_t task_id )
    {
        au::TokenTaker tk( &token );
        
        // Create the upload operation
        ControllerUploadOperation * uploadOperation = new ControllerUploadOperation( task_id );
        
        // Add to out local map
        uploads.insertInMap( task_id , uploadOperation );
        
        
    }
    
    void ControllerLoadManager::addDownload( size_t task_id )
    {
        au::TokenTaker tk( &token );
        
        // Create the upload operation
        ControllerDownloadOperation * downloadOperation = new ControllerDownloadOperation( task_id );
        
        // Add to out local map
        downloads.insertInMap( task_id , downloadOperation );
        
        
    }
    
    ControllerUploadOperation* ControllerLoadManager::extractUploadOperation( size_t load_id )
    {
        au::TokenTaker tk( &token );
        return  uploads.extractFromMap( load_id );
    }
    
    ControllerDownloadOperation* ControllerLoadManager::extractDownloadOperation( size_t load_id )
    {
        au::TokenTaker tk( &token );
        return downloads.extractFromMap( load_id );
    }
    
    
    // Fill the active upload / download operation to avoid removing files
    void ControllerLoadManager::fill( samson::network::QueueList *ql )
    {
        au::TokenTaker tk( &token );
        
        for ( au::map< size_t , ControllerUploadOperation >::iterator i = uploads.begin() ; i != uploads.end() ; i++)
            ql->add_load_id ( i->first );	 // get the id of the operation
        
    }
    
}
