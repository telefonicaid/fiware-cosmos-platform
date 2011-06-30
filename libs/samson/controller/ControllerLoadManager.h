#ifndef CONTROLLER_LOAD_MANAGER
#define CONTROLLER_LOAD_MANAGER

#include "au/Token.h"		// au::Token

namespace samson
{

  /**
     Information about upload and download operations
  */

  class ControllerLoadOperation
  {
	public:

	  size_t task_id;    // Identifier of the task at ControllerDataManager

		ControllerLoadOperation( size_t _task_id )
		{
		  task_id = _task_id;
		}
  };

  class ControllerUploadOperation : public ControllerLoadOperation
  {
  public:
      ControllerUploadOperation(  size_t task_id ) : ControllerLoadOperation( task_id )
      {
      }
  };

  class ControllerDownloadOperation : public ControllerLoadOperation
  {
  public:
      ControllerDownloadOperation( size_t task_id ) : ControllerLoadOperation( task_id )
      {
      }
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

    ControllerLoadManager() : token( "ControllerLoadManager" )
    {
    }
      
    void addUpload( size_t task_id )
    {
        au::TokenTaker tk( &token );

		// Create the upload operation
		ControllerUploadOperation * uploadOperation = new ControllerUploadOperation( task_id );

		// Add to out local map
		uploads.insertInMap( task_id , uploadOperation );
		

    }

    void addDownload( size_t task_id )
    {
        au::TokenTaker tk( &token );
		
		// Create the upload operation
		ControllerDownloadOperation * downloadOperation = new ControllerDownloadOperation( task_id );
		
		// Add to out local map
		downloads.insertInMap( task_id , downloadOperation );

		
	}

	ControllerUploadOperation* extractUploadOperation( size_t load_id )
	{
        au::TokenTaker tk( &token );
		return  uploads.extractFromMap( load_id );
	}
	
	ControllerDownloadOperation* extractDownloadOperation( size_t load_id )
	{
        au::TokenTaker tk( &token );
		return downloads.extractFromMap( load_id );
	}
	  
	  
	  // Fill the active upload / download operation to avoid removing files
	  void fill( samson::network::QueueList *ql )
	  {
          au::TokenTaker tk( &token );
		  
		  for ( au::map< size_t , ControllerUploadOperation >::iterator i = uploads.begin() ; i != uploads.end() ; i++)
			  ql->add_load_id ( i->first );	 // get the id of the operation
		  
	  }
	  
  };


}


#endif
