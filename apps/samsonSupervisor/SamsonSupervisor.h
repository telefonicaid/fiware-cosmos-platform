#ifndef SAMSON_SUPERVISOR_H
#define SAMSON_SUPERVISOR_H

/* ****************************************************************************
*
* FILE                     SamsonSupervisor.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Dec 15 2010
*
*/
#include <QWidget>

#include "ports.h"              // LOG_MESSAGE_PORT
#include "NetworkInterface.h"   // DataReceiverInterface, EndpointUpdateInterface
#include "Endpoint.h"           // Endpoint
#include "Network.h"            // Network
#include "Message.h"            // ss::Message::Header
#include "Delilah.h"			// ss::Delilah



/* ****************************************************************************
*
* SamsonSupervisor - 
*/
class SamsonSupervisor : public ss::Delilah, public ss::DataReceiverInterface, public ss::EndpointUpdateReceiverInterface, public ss::ReadyReceiverInterface, public QWidget
{
public:
	SamsonSupervisor(ss::Network* netP) : ss::Delilah(netP, false)
	{
		networkP = netP; 

		logReceiverInit(LOG_MESSAGE_PORT);

		LM_T(LmtQtTimer, ("Starting timer for Network polling"));
		startTimer(1000);  // 1 second timer (was 10 ms)
	}

	virtual int receive(int fromId, int nb, ss::Message::Header* headerP, void* dataP);
	virtual int endpointUpdate(ss::Endpoint* ep, ss::Endpoint::UpdateReason reason, const char* reasonText, void* info = NULL);
	virtual int ready(const char* info);

	
	// Function to overload in Delilah
	
	// Function to be implemented by sub-classes to process packets ( not handled by this class )
	virtual int _receive(int fromId, ss::Message::MessageCode msgCode, ss::Packet* packet){return 0;};
	
	// A load data process has finished
	virtual void loadDataConfirmation( ss::DelilahUploadDataProcess *process ){};
	
	// Write something on screen
	virtual void showMessage(std::string message){};
	
	// Callback to notify that a particular operation has finished
	virtual void notifyFinishOperation( size_t ){};
	
private:
	ss::Network*    networkP;


protected:
	void timerEvent(QTimerEvent* e);

	int  logSocket;
	void logReceiverInit(unsigned short port);
	void logReceive(void);
};

#endif
