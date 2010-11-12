/*
 * DelilahQtApp.cpp
 *
 *  Created on: Nov 4, 2010
 *      Author: ania
 */

#include "Packet.h"
#include "Message.h"

#include "DelilahQtApp.h"
#include "MainWindow.h"
#include "Delilah.h"

#include "Queue.h"

DelilahQtApp::DelilahQtApp(int &argc, char ** argv, ss::Delilah* _delilah)
	: QApplication(argc, argv)
{
	id = 0;

	delilah = _delilah;
	w = new MainWindow();			// My main window interface
	w->show();

	connect(this, SIGNAL(lastWindowClosed()), this, SLOT(quitDelilah()));

	// TODO: This should be called after connection is established
	uploadData(true, true, true);
}

void DelilahQtApp::uploadData(bool queues, bool operations, bool data_types)
{
	// Ask for all help
	ss::Packet p;
	ss::network::Help *help = p.message.mutable_help();
	help->set_queues(queues);
	help->set_datas(data_types);
	help->set_operations(operations);
	delilah->network->send(delilah, delilah->network->controllerGetIdentifier(), ss::Message::Help, &p);
}

void DelilahQtApp::sendCreateQueue(const QString &name)
{
	// TODO:
	std::string command = "add_data_queue ";
	command.append(name.toStdString());

	ss::Packet p;
	ss::network::Command *c = p.message.mutable_command();
	c->set_command( command );
	c->set_sender_id( id++ );
	delilah->network->send(delilah, delilah->network->controllerGetIdentifier(), ss::Message::Command, &p);

	std::cout << "SENDING request\n";
}

void DelilahQtApp::sendCreateQueue(const QString &name, const QString &key_type, const QString &value_type)
{
	// TODO:
}

/*
 * Called when packet with help response was received.
 * Packet can contains information about all (or only some of) objects: queues, data queues,
 * operations, and data types currently available in the system.
 * This information is extracted and appropriate lists are updated.
 */
int DelilahQtApp::receiveData(ss::Packet* packet)
{
	ss::network::HelpResponse resp = packet->message.help_response();

	if( resp.queues() )
	{
		for (int i=0 ; i<resp.queue_size(); i++)
		{
			ss::network::Queue q = resp.queue(i);

			KVQueue* queue = new KVQueue(QString::fromStdString(q.name()));
			queue->setSize( q.info().size() );
			queue->setKey( QString::fromStdString(q.format().keyformat()) );
			queue->setValue( QString::fromStdString(q.format().valueformat()) );
			queue->setKVNumber(q.info().kvs());
			kv_queues.append(queue);
		}

		for (int i=0 ; i<resp.data_queue_size(); i++)
		{
			ss::network::DataQueue q = resp.data_queue(i);

			DataQueue* queue = new DataQueue(QString::fromStdString(q.name()));
			queue->setSize(q.size());
			data_queues.append(queue);
		}

	}

	// TODO:
	// Load operations
	// Load data types
	return 0;
}

int DelilahQtApp::receiveCommandResponse(size_t id, ss::Packet* packet)
{
	return 0;
}

int DelilahQtApp::receiveUknownPacket(size_t id, ss::Message::MessageCode msgCode, ss::Packet* packet)
{
	return 0;
}

void DelilahQtApp::quitDelilah()
{
	delilah->quit();
}

void DelilahQtApp::receivedMessage(size_t id, bool error, bool finished, std::string message)
{
	QString m;
	m = m.fromStdString(message);
	if (finished)
	{
		emit(jobFinished(id, error, m));
	}
	else
		emit(jobUpdated(id, error, m));
}

/*
 * Checks if the given name can be used for creating a new queue.
 * Returns a null string if the name is valid or description why the name cannot be used
 * for new queue creation.
 */
QString DelilahQtApp::validateNewQueueName(QString name)
{
	// TODO: implement me!!!!!!!!!!!!!
	// Just for testing
	if (name=="queue")
		return QString("Queue can not have such name.");
	if (name=="test")
		return QString("Such name already exists.");

	return QString();
}

DataQueue* DelilahQtApp::getDataQueue(const QString &name)
{
	// TODO:
	DataQueue* q = new DataQueue(name);
	return q;
}

/******************************************************************************
 *
 * Private Methods
 *
 *****************************************************************************/

size_t DelilahQtApp::sendMessage(QString command)
{
//	return client->sendMessageToController( command.toStdString() );
	return 1;
}

