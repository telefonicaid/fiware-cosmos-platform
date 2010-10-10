/* ****************************************************************************
 *
 * FILE                     network.cpp - Definition for the network interface
 */


/* ****************************************************************************
 *
 *
 */


#include "network.h"			// Own interface
#include "endpoint.h"			// EndPoint
#include "packet.h"				// Packet

namespace ss
{

	NetworkInterface::NetworkInterface(  )
	{
		receiver = NULL;
	}

	void NetworkInterface::setPacketReceiver( PacketReceiverInterface *_receiver )
	{
		receiver = _receiver;
	}

	
	void NetworkInterface::initAsSamsonController( EndPoint myEndPoint , std::vector<EndPoint> peers ){}	
	void NetworkInterface::initAsSamsonWorker(EndPoint myEndPoint ,  EndPoint controllerEndPoint ){}
	void NetworkInterface::initAsDelailah( EndPoint controllerEndPoint ){}
	
	
	EndPoint NetworkInterface::me()
	{
		return EndPoint();
	}
	
	EndPoint NetworkInterface::controller()
	{
		return EndPoint();
	}
	
	EndPoint NetworkInterface::worker( int i )
	{
		return EndPoint();
	}
	
	
	int NetworkInterface::worker( EndPoint endPoint )
	{
		return -1;
	}
	
	
	std::vector<EndPoint> NetworkInterface::endPoints()
	{
		std::vector<EndPoint> v;
		return v;
	}
	
	std::vector<EndPoint> NetworkInterface::samsonWorkersPoints()
	{
		std::vector<EndPoint> v;
		return v;
	}
	
	
	size_t NetworkInterface::send( Packet *p , EndPoint endPoint , PacketSenderInterface *sender )
	{
		// Rigth now, local loop
		receiver->receive(p, endPoint);
		
		if( sender )
			sender->notificationSent(0, true);
		
		return 0;
	}
	
	bool NetworkInterface::ready()
	{
		return false;
	}
	
	
}