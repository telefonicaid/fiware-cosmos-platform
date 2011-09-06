#ifndef CONTROLLER_LOAD_MANAGER
#define CONTROLLER_LOAD_MANAGER

#include "au/Token.h"		// au::Token
#include "au/string.h"      // au::xml_...

#include "samson/common/samson.pb.h"        // samson::network..

namespace samson
{
    
    /**
     Information about upload and download operations
     */
    
    class ControllerLoadOperation
    {
	public:
        
        size_t task_id;    // Identifier of the task at ControllerDataManager
        
		ControllerLoadOperation( size_t _task_id );
        virtual void getInfo( std::ostringstream& output){};
        
    };
    
    class ControllerUploadOperation : public ControllerLoadOperation
    {
    public:
        
        ControllerUploadOperation(  size_t task_id );
        void getInfo( std::ostringstream& output);
        
    };
    
    class ControllerDownloadOperation : public ControllerLoadOperation
    {
    public:
        
        ControllerDownloadOperation( size_t task_id );        
        void getInfo( std::ostringstream& output);
    };
    
    
    /**
     Class to keep state of the upload and download operations at the controller
     It basically keep information of the tasks to cancel them after a timeout
     */
    
    class ControllerLoadManager
    {
        
        au::Token token; // Internal token to protect multi-thread access
        
        au::map< size_t , ControllerUploadOperation > uploads;
        au::map< size_t , ControllerDownloadOperation > downloads;
        
    public:
        
        ControllerLoadManager();        

        void getInfo( std::ostringstream& output);
        
        void addUpload( size_t task_id );
        void addDownload( size_t task_id );
        
        ControllerUploadOperation* extractUploadOperation( size_t load_id );
        ControllerDownloadOperation* extractDownloadOperation( size_t load_id );
        
        // Fill the active upload / download operation to avoid removing files
        void fill( samson::network::QueueList *ql );
        
    };
    
    
}


#endif
