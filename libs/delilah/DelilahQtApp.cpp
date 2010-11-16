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
#include "globals.h"

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

void DelilahQtApp::uploadData(bool queues, bool operations, bool data_types, const QString &name)
{
	// Ask for all help
	ss::Packet p;
	ss::network::Help *help = p.message.mutable_help();
	help->set_queues(queues);
	help->set_datas(data_types);
	help->set_operations(operations);
	// TODO: add set name
	delilah->network->send(delilah, delilah->network->controllerGetIdentifier(), ss::Message::Help, &p);
}

int DelilahQtApp::sendCreateDataQueue(const QString &name)
{
	std::string command = "add_data_queue ";
	command.append(name.toStdString());

	ss::Packet p;
	ss::network::Command *c = p.message.mutable_command();
	c->set_command( command );
	c->set_sender_id( ++id );
	delilah->network->send(delilah, delilah->network->controllerGetIdentifier(), ss::Message::Command, &p);

	return id;
}

int DelilahQtApp::sendCreateKVQueue(const QString &name, const QString &key_type, const QString &value_type)
{
	// TODO:
	++id;

	return id;
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

//	if (resp.name().empty())
//		// if the help command was send to get info about all objects, load new data
//		return loadData(packet);
//	else
//		// otherwise update one item
//		return updateData(packet);
	return loadData(packet);
}

int DelilahQtApp::loadData(ss::Packet* packet)
{
	ss::network::HelpResponse resp = packet->message.help_response();

	if( resp.queues() )
	{
		// TODO:
		kv_queues.clear();
		data_queues.clear();

		for (int i=0 ; i<resp.queue_size(); i++)
		{
			ss::network::Queue q = resp.queue(i);
			addKVQueue(q);
		}
		for (int i=0 ; i<resp.data_queue_size(); i++)
		{
			ss::network::DataQueue q = resp.data_queue(i);
			addDataQueue(q);
		}
	}

	// TODO:
	// Load operations
	// Load data types
	return 0;
}

int DelilahQtApp::updateData(ss::Packet* packet)
{
	ss::network::HelpResponse resp = packet->message.help_response();

	return 0;
}

void DelilahQtApp::addKVQueue(ss::network::Queue q)
{
	KVQueue* queue = new KVQueue(QString::fromStdString(q.name()));
	queue->setSize( q.info().size() );
	queue->setKey( QString::fromStdString(q.format().keyformat()) );
	queue->setValue( QString::fromStdString(q.format().valueformat()) );
	queue->setKVNumber(q.info().kvs());
	kv_queues.append(queue);
}

void DelilahQtApp::addDataQueue(ss::network::DataQueue q)
{
	DataQueue* queue = new DataQueue(QString::fromStdString(q.name()));
	queue->setSize(q.size());
	data_queues.append(queue);
}

int DelilahQtApp::receiveCommandResponse(ss::Packet* packet)
{
	unsigned int id = packet->message.command_response().sender_id();
	QString command = QString::fromStdString(packet->message.command_response().command());
	QString message = QString::fromStdString(packet->message.command_response().response());
	bool error = packet->message.command_response().error();
	bool finished = packet->message.command_response().finish();

	if(finished && !error)
	{
		// Depending on the finished command update information about queues/operations/data type
		// available on the system
		QStringList args = command.split(" ");


		if (args[0]==CREATE_DATA_QUEUE_COMMAND)
		{
			// TODO:
			// Create DataQueue with default values and add it to data_queues list
			DataQueue* q = new DataQueue(args[1]);
			data_queues.append(q);

			// Update newly created DataQueue with real data got from SMAMSON platform
			uploadData(true, false, false, args[1]);
		}
	}

	emit(gotCommandResponse(id, finished, error, message));

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

