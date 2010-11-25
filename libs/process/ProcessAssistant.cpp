/* ****************************************************************************
*
* FILE                     ProcessAssistant.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Nov 5 2010
*
*/
#include <time.h>                  // time
#include <pthread.h>               // pthread_t

#include "logMsg.h"                // LM_*
#include "Alarm.h"                 // ALARM
#include "processTraceLevels.h"    // LMT_*

#include "Message.h"               // Message::*
#include "iomServerOpen.h"         // iomServerOpen
#include "iomConnect.h"            // iomConnect
#include "iomAccept.h"             // iomAccept
#include "iomMsgAwait.h"           // iomMsgAwait
#include "iomMsgSend.h"            // iomMsgSend
#include "iomMsgRead.h"            // iomMsgRead
#include "Endpoint.h"              // Endpoint
#include "SamsonWorker.h"          // SamsonWorker
#include "Process.h"               // Process
#include "ProcessAssistant.h"      // Own interface
#include "SamsonWorker.h"			// ss::SamsonWorker
#include "ProcessWriter.h"			// ss::ProcessAssistantOperationFramework
#include "ProcessAssistantOperationFramework.h"	// ss::ProcessAssistantOperationFramework
#include "WorkerTaskItem.h"			// ss::WorkerTaskItem
	
namespace ss {
	
		/* ****************************************************************************
		 *
		 * runThread - 
		 */
		
		static void* runProcessAssistantThread(void* vP)
		{
			ProcessAssistant* paP = (ProcessAssistant*) vP;
			paP->run();
			return NULL;
		}	
		
		ProcessAssistant::ProcessAssistant( int coreNo, SamsonWorker* worker ) :  ProcessAssistantBase( coreNo, worker )
		{
			
			// Create the main thread for the process assistant
			pthread_create(&threadId, NULL, runProcessAssistantThread, this);
			item = NULL;
		}
		
		void ProcessAssistant::run()
		{
			
			// Main thread to get items and perform actions
			while( true )
			{
				setStatus( "Asking for the next task..." );
				
				// Get the next item to process ( this call is blocked if no task is available )
				item =  worker->taskManager.getNextItemToProcess();

				setStatus( "Running..." + item->getStatus() );
				
				// Run whatever is required in this item offering ProcessAssistant to use Process
				item->run( this );
				
				// Notify that this item is done
				worker->taskManager.finishItem( item , false , "");
				
				// Put item to null to avoid old notifications from worker
				item = NULL;
			}
		}
	
		void ProcessAssistant::receiveCommand( network::ProcessMessage p )
		{
			assert(item);
			item->receiveCommand( this, p );
		}
	
	
}
