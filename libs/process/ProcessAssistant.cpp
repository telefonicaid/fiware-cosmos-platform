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
#include "WorkerTask.h"				// ss::WorkerTask

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
		
		ProcessAssistant::ProcessAssistant( int coreNo, WorkerTaskManager* _taskManager ) :  ProcessAssistantBase( coreNo, _taskManager )
		{
			
			// Create the main thread for the process assistant
			pthread_create(&threadId, NULL, runProcessAssistantThread, this);
			item = NULL;
			
			working = false;
		}
		
		void ProcessAssistant::run()
		{
			
			// Main thread to get items and perform actions
			while( true )
			{
				setStatus( "Waiting for the next task" );
				working = false;

				// Get the next item to process ( this call is blocked if no task is available )
				item =  taskManager->getNextItemToProcess();

				setStatus( "Running " + item->task->operation );
				working = true;
				
				// Run whatever is required in this item offering ProcessAssistant to use Process
				item->run( this );

				setStatus( "Finishing " + item->task->operation );
				
				// Notify that this item is done
				taskManager->finishItem( item );
				
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
