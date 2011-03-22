
#ifndef _H_ENGINE_DELEGATES
#define _H_ENGINE_DELEGATES

#include "EngineElement.h"		// ss::EngineElement
#include "DiskOperation.h"		// ss::DiskOperation
#include "ProcessItem.h"		// ss::ProcessItem
#include "MemoryRequest.h"		// ss::MemoryRequest

namespace ss {
	
	
	/**
	 Callback for DiskOperation
	 */
	
	class DiskManagerDelegate
	{
		// Friend class to call this method only inside Engine
		friend class DiskManagerNotification;
		
		// Notify that this operation has finished
		virtual void diskManagerNotifyFinish(  DiskOperation *operation ) = 0;	
	};

	/**
	 Callback for a MemoryRequest  
	 */
	
	class MemoryRequestDelegate
	{
	public:
		virtual void notifyFinishMemoryRequest( MemoryRequest *request )=0;
	};
	
	
	/**
	 Callback for a ProcessItem
	 */
	
	class ProcessManagerDelegate
	{
		friend class ProcessManagerNotification;
		virtual void notifyFinishProcess( ProcessItem * item ) = 0;
	};

	/**
	 Classes for using callbacks inside Engine
	 **/
	
	class MemoryRequestNotification : public EngineElement
	{
		MemoryRequest *request;
	public:
		MemoryRequestNotification( MemoryRequest * _request )
		{
			request = _request;
			description = "MemoryRequestNotification";
		}
		
		void run()
		{
			if( !request->delegate )
				LM_X(1, ("Memory Manager: It is not allowed to requets memory without delegate... no sense"));
			
			request->delegate->notifyFinishMemoryRequest(request);
			delete request;
		}
	};	
	
	
	class ProcessManagerNotification : public EngineElement
	{
		
		ProcessItem *item;
		
	public:
		
		ProcessManagerNotification( ProcessItem *_item )
		{
			item = _item;
		}
		
		void run()
		{
			if( item->delegate )
				item->delegate->notifyFinishProcess(item);
			else
				delete item;
		}		
	};	
	
	
	/**
	 Class to notify the end of a disk operation
	 */
	
	class DiskManagerNotification : public EngineElement
	{
		DiskOperation *operation;
		
	public:
		
		DiskManagerNotification( DiskOperation *_operation )
		{
			operation = _operation;
		}
		
		void run()
		{
			if( operation->delegate ) 
				operation->delegate->diskManagerNotifyFinish( operation );
			delete operation;
		}
		
	};

}

#endif