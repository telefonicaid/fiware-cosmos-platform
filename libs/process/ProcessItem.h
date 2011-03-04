#ifndef _H_PROCESS_ITEM
#define _H_PROCESS_ITEM


#include <string>
#include <sstream>

/**
 
 ProcessItem
 
	item inside the ProcessManager
 
	An item is a task to do without any IO operation in the middle.
	It only requires processing time
 */

namespace ss {

	
	class ProcessItem;
	
	/**
	 Delegate interface to receive notifications about finished ProcessItems
	 */
	
	class ProcessManagerDelegate
	{
	public:
		virtual void notifyFinishProcess( ProcessItem * item ) = 0;
	};
	
	
	class ProcessItem  
	{
		// Delegate to notify when finishing
		ProcessManagerDelegate * delegate;

		std::string status_letter;
		std::string status;
		
	protected:
		double progress;		// Progress of the operation ( if internally reported somehow )
	public:
		
		// Error management
		bool error;
		std::string error_message;

		
		// Identifier in the Process manager ( for debuggin mainly )
		size_t id_processItem;
		
		
		void setError( std::string _error_message )
		{
			error = true;
			error_message = _error_message;
		}
		
	public:

		size_t component;		// Used by clients to indentify the component inside delegate to receive notiffication
		size_t tag;				// Used by clients to identify element inside delegate
		size_t sub_tag;			// Used by clients to identify sub-element inside delegate
		
		ProcessItem( ProcessManagerDelegate * _delegate )
		{
			delegate = _delegate;
			error =  false;	// No error by default
			
			status_letter =  "R";
			status = "unknown";	// Default message for the status
			
			progress = 0;	// Initial progress to "0"
		}
		
		
		std::string getStatus();
		
		void setStatus(std::string _status)
		{
			status =  _status;
		}
		
		void setStatusLetter( std::string _status_letter)
		{
			status_letter = _status_letter;
		}
		
		ProcessItem()
		{
			delegate = NULL;
			error =  false;	// No error by default
		}
		
		void setProcessManagerDelegate( ProcessManagerDelegate * _delegate )
		{
			delegate = _delegate;
		}
		
		void notifyFinishToDelegate()
		{
			if( delegate )
				delegate->notifyFinishProcess(this);
			else
			{
				// Autodelete if there is no delegate to handle notification
				delete this;
			}
		}
		
		// What to do when processor is available
		virtual void run()=0;
	};
	

}

#endif
