/*
 * DelilahQtApp.cpp
 *
 *  Created on: Nov 4, 2010
 *      Author: ania
 */

#include <cassert>


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
	uploadData(true, true, true, true);
}

/*
 * Sends request to the network to get information about all objects (queues, operations, and data types)
 * available on the server.
 */
void DelilahQtApp::uploadData(bool data_queue, bool kv_queue, bool operation, bool data_type, const QString &name)
{
	// set status of the objects (currently only queues) that are going to be uploaded
	// to SYNCHRONIZING.
	if (name.isEmpty())
	{
		// set status of all items to SYNCHRONIZING
		if (data_queue)
			for(int i=0; i< data_queues.size(); i++)
				data_queues.at(i)->setStatus(Queue::SYNCHRONIZING);
		if (kv_queue)
			for(int i=0; i< kv_queues.size(); i++)
				kv_queues.at(i)->setStatus(Queue::SYNCHRONIZING);
	}
	else
	{
		if (data_queue)
		{
			DataQueue *data_q = getDataQueue(name);
			if (data_q)
				data_q->setStatus(Queue::SYNCHRONIZING);
		}
		if (kv_queue)
		{
			KVQueue *kv_q = getKVQueue(name);
			if (kv_q)
				kv_q->setStatus(Queue::SYNCHRONIZING);
		}
	}


	// Prepare packet to get information from network about requested type of objects
	ss::Packet p;
	ss::network::Help *help = p.message.mutable_help();
	help->set_data_queues(data_queue);
	help->set_queues(kv_queue);
	help->set_datas(data_type);
	help->set_operations(operation);
	if ( !name.isEmpty() )
		help->set_name(name.toStdString());
	// TODO: add set name
	delilah->network->send(delilah, delilah->network->controllerGetIdentifier(), ss::Message::Help, &p);
}

int DelilahQtApp::sendCreateDataQueue(const QString &name)
{
	std::string command = CREATE_DATA_QUEUE_COMMAND;
	command.append((" " + name).toStdString());

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

int DelilahQtApp::sendDeleteDataQueue(const QString &name)
{
	std::string command = REMOVE_DATA_QUEUE_COMMAND;
	command.append((" " + name).toStdString());

	ss::Packet p;
	ss::network::Command *c = p.message.mutable_command();
	c->set_command( command );
	c->set_sender_id( ++id );
	delilah->network->send(delilah, delilah->network->controllerGetIdentifier(), ss::Message::Command, &p);

	return id;
}

int DelilahQtApp::sendDeleteKVQueue(const QString &name)
{
	std::string command = REMOVE_KV_QUEUE_COMMAND;
	command.append((" " + name).toStdString());

	ss::Packet p;
	ss::network::Command *c = p.message.mutable_command();
	c->set_command( command );
	c->set_sender_id( ++id );
	delilah->network->send(delilah, delilah->network->controllerGetIdentifier(), ss::Message::Command, &p);

	return id;
}

/*
 * Called when packet with help response was received.
 * Packet can contains information about KV queues, data queues, operations, and data types
 * currently available in the system.
 * This information is extracted from the packet and appropriate application's lists are updated.
 */
int DelilahQtApp::receiveData(ss::Packet* packet)
{
	// TODO: remove!!!!!1 It's for debugging and testing
	sleep(2);
	std::cout << "Sleeping 2 sec. in receiveData" << std::endl;

	ss::network::HelpResponse resp = packet->message.help_response();
	ss::network::Help command = resp.help();

	// set flag if synchronization applies to the whole list or to only
	// one item (help command contained name argument)
	bool synchronize_all = false;
	if (command.name().empty())
		synchronize_all = true;

	if (command.data_queues())
		synchronizeDataQueues(resp, synchronize_all);
	if (command.queues())
		synchronizeKVQueues(resp, synchronize_all);
	if (command.operations())
		synchronizeOperations(resp, synchronize_all);
	if (command.datas())
		synchronizeDataTypes(resp, synchronize_all);
	return 0;
}

int DelilahQtApp::receiveCommandResponse(ss::Packet* packet)
{
	// TODO: remove!!!!!1 It's for debugging and testing
	sleep(2);
	std::cout << "Sleeping 2 sec. in receiveCommandResponse" << std::endl;

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
			assert(args.size()>1);

			// Create DataQueue with default values and add it to data_queues list
			DataQueue* q = new DataQueue(args[1]);
			data_queues.append(q);

			// Update newly created DataQueue with real data got from SMAMSON platform
			uploadData(true, false, false, false, args[1]);
		}

		if (args[0]==CREATE_KV_QUEUE_COMMAND)
		{
			// TODO:
		}

		if (args[0]==REMOVE_DATA_QUEUE_COMMAND)
		{
			assert(args.size()>1);

			// Mark queue as deleted
			DataQueue* q = getDataQueue(args[1]);
			if (q)
				q->setStatus(Queue::DELETED);
		}

		if (args[0]==REMOVE_KV_QUEUE_COMMAND)
		{
			// TODO:
		}
	}

	emit(gotCommandResponse(id, finished, error, message));

	return 0;
}

int DelilahQtApp::receiveUknownPacket(size_t id, ss::Message::MessageCode msgCode, ss::Packet* packet)
{
	// TODO:
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

/*
 * Search for data queue with a given name.
 * Parameter 'deleted' indicates whether search will be done for existing
 * or deleted queues. By default 'deleted' is set to false, and search goes only
 * through existing queues.
 */
DataQueue* DelilahQtApp::getDataQueue(const QString &name, bool deleted)
{
	for(int i=0; i<data_queues.size(); i++)
	{
		DataQueue* queue = data_queues.at(i);
		if (queue->getName().compare(name) == 0)
		{
			if ( (deleted && queue->getStatus()==Queue::DELETED) ||
					!(deleted || queue->getStatus()==Queue::DELETED) )
				return queue;
		}
	}

	return 0;
}

/*
 * Search for data queue with a given name.
 * Parameter 'deleted' indicates whether search will be done for existing
 * or deleted queues. By default 'deleted' is set to false, and search goes only
 * through existing queues.
 */
KVQueue* DelilahQtApp::getKVQueue(const QString &name, bool deleted)
{
	// TODO:
	for(int i=0; i<kv_queues.size(); i++)
	{
		KVQueue* queue = kv_queues.at(i);
		if (queue->getName().compare(name) == 0)
		{
			if ( (deleted && queue->getStatus()==Queue::DELETED) ||
					!(deleted || queue->getStatus()==Queue::DELETED) )
				return queue;
		}
	}

	return 0;
}

/******************************************************************************
 *
 * Private methods
 *
 *****************************************************************************/

/*
 * Synchronizes data_queues list with the queues returned in HelpResponse.
 * If synchronize_all is true, all queues that were not updated (have status SYNCHRONIZING)
 * are marked as deleted.
 */
void DelilahQtApp::synchronizeDataQueues(const ss::network::HelpResponse &resp, bool synchronize_all)
{
	for (int i=0 ; i<resp.data_queue_size(); i++)
	{
		// TODO: remove!!!!!!!!
		std::cout << "number of data_queues: " << resp.data_queue_size() << std::endl;


		ss::network::Queue q = resp.data_queue(i);
		QString name = QString::fromStdString(q.name());

		// find if queue with such name already exists. If not, create
		// the new one and add it to the global list.
		DataQueue *queue = getDataQueue(name);
		if (queue==0)
		{
			queue = new DataQueue(name);
			data_queues.append(queue);
		}

		// Upload queue's new values
		queue->upload(&q);
	}

	// If synchronization applies to the whole list, queues that where not uploaded
	// have status set to DELETED
	// We do not remove this queues from the list, in case they are currently used
	// in some workspace.
	if (synchronize_all)
	{
		for (int i=0; i<data_queues.size(); i++)
		{
			if ( data_queues.at(i)->getStatus() == Queue::SYNCHRONIZING )
				data_queues.at(i)->setStatus(Queue::DELETED);
		}
	}
}

/*
 * Synchronizes key-value queues list with the queues returned in HelpResponse.
 * If synchronize_all is true, all queues that were not updated (have status SYNCHRONIZING)
 * are marked as deleted.
 */
void DelilahQtApp::synchronizeKVQueues(const ss::network::HelpResponse &resp, bool synchronize_all)
{
	for (int i=0 ; i<resp.queue_size(); i++)
	{
		ss::network::Queue q = resp.queue(i);
		QString name = QString::fromStdString(q.name());

		// find if queue with such name already exists. If not, create
		// the new one and add it to the global list.
		KVQueue *queue = getKVQueue(name);
		if (queue==0)
		{
			queue = new KVQueue(name);
			kv_queues.append(queue);
		}

		// Upload queue's new values
		queue->upload(&q);
	}

	// If synchronization applies to the whole list, queues that where not uploaded
	// have status set to DELETED
	// We do not remove this queues from the list, in case they are currently used
	// in some workspace.
	if (synchronize_all)
	{
		for (int i=0; i<kv_queues.size(); i++)
		{
			if ( kv_queues.at(i)->getStatus() == Queue::SYNCHRONIZING )
				kv_queues.at(i)->setStatus(Queue::DELETED);
		}
	}
}

void DelilahQtApp::synchronizeOperations(const ss::network::HelpResponse &resp, bool synchronize_all)
{
	// TODO:
}

void DelilahQtApp::synchronizeDataTypes(const ss::network::HelpResponse &resp, bool synchronize_all)
{
	// TODO:
}
