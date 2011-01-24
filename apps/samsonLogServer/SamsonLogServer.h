#ifndef SAMSON_LOG_SERVER_H
#define SAMSON_LOG_SERVER_H

/* ****************************************************************************
*
* FILE                     SamsonLogServer.h - 
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jan 24 2011
*
*/
#include <QObject>
#include <QTimerEvent>

#include "NetworkInterface.h"   // DataReceiverInterface, EndpointUpdateInterface
#include "Endpoint.h"           // Endpoint
#include "Network.h"            // Network
#include "Message.h"            // ss::Message::Header



/* ****************************************************************************
*
* SamsonLogServer - 
*/
class SamsonLogServer : public QObject, public ss::DataReceiverInterface, public ss::EndpointUpdateReceiverInterface, public ss::ReadyReceiverInterface
{
public:
	SamsonLogServer(void);

	virtual int receive(int fromId, int nb, ss::Message::Header* headerP, void* dataP);
	virtual int endpointUpdate(ss::Endpoint* ep, ss::Endpoint::UpdateReason reason, const char* reasonText, void* info = NULL);
	virtual int ready(const char* info);

protected:
	void timerEvent(QTimerEvent* e);
};

#endif
