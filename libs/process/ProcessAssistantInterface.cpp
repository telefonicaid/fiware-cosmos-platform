
#include "ProcessAssistantInterface.h"		// Own interface
#include "SamsonWorker.h"					// ss::SamsonWorker
#include "Packet.h"							// ss::Packet

namespace ss
{
	ProcessAssistantInterface::ProcessAssistantInterface( int _core , SamsonWorker* _worker )
	{
		status = "no status defined...";
		core = _core;
		worker = _worker;
	}

	void ProcessAssistantInterface::sendCloseMessages( size_t task_id, int workers )
	{
		
		for (int s = 0 ; s < workers ; s++)
		{				
			Packet p;
			network::WorkerDataExchangeClose *dataMessage =  p.message.mutable_data_close();
			dataMessage->set_task_id(task_id);
			NetworkInterface *network = worker->network;
			network->send(worker, network->workerGetIdentifier(s) , Message::WorkerDataExchangeClose, &p);
		}
	}	
	
}