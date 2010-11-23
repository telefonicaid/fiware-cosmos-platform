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
	uploadData(true, true, true);
}

/*
 * Sends request to the network to get information about all objects (queues, operations, and data types)
 * available on the server.
 */
void DelilahQtApp::uploadData(bool queue, bool operation, bool data_type, const QString &name)
{
	// set status of the objects (currently only queues) that are going to be uploaded
	// to SYNCHRONIZING.
	if (name.isEmpty())
	{
		// set status of all queues to SYNCHRONIZING
		if (queue)
			for(int i=0; i<queues.size(); i++)
				queues.at(i)->setStatus(Queue::SYNCHRONIZING);
	}
	else
	{
		if (queue)
		{
			Queue *q = getQueue(name);
			if (q)
				q->setStatus(Queue::SYNCHRONIZING);
		}
	}


	// Prepare packet to get information from network about requested type of objects
	ss::Packet *p = new ss::Packet();
	ss::network::Help *help = p->message.mutable_help();
	help->set_queues(queue);
	help->set_datas(data_type);
	help->set_operations(operation);
	if ( !name.isEmpty() )
		help->set_name(name.toStdString());
	delilah->network->send(delilah, delilah->network->controllerGetIdentifier(), ss::Message::Help, p);
}

int DelilahQtApp::sendCreateQueue(const QString &name)
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

int DelilahQtApp::sendCreateQueue(const QString &name, const QString &key_type, const QString &value_type)
{
	// TODO:
	++id;

	return id;
}

int DelilahQtApp::sendDeleteQueue(const QString &name)
{
	std::string command = REMOVE_QUEUE_COMMAND;
	command.append((" " + name).toStdString());

	ss::Packet *p = new ss::Packet();
	ss::network::Command *c = p->message.mutable_command();
	c->set_command( command );
	c->set_sender_id( ++id );
	delilah->network->send(delilah, delilah->network->controllerGetIdentifier(), ss::Message::Command, p);

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

	if (command.queues())
		synchronizeQueues(resp, synchronize_all);
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
			Queue* q = new Queue(args[1], DATA_QUEUE);
			queues.append(q);

			// Update newly created DataQueue with real data got from SMAMSON platform
			uploadData(true, false, false, args[1]);
		}

		if (args[0]==CREATE_KV_QUEUE_COMMAND)
		{
			// TODO:
		}

		if (args[0]==REMOVE_QUEUE_COMMAND)
		{
			assert(args.size()>1);

			// Mark queue as deleted
			Queue* q = getQueue(args[1]);
			if (q)
				q->setStatus(Queue::DELETED);
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
 * Return list of all queues available in the system.
 * Argument 'deleted' indicates if returned list will contain queues
 * currently available in the system or queues deleted from the system
 * after the last startup of the application.
 * Argument 'deleted' has deafult value set to false.
 */
QList<Queue*> DelilahQtApp::getQueues(bool deleted)
{
	QList<Queue*> selected;
	for(int i=0; i<queues.size(); i++)
	{
		Queue* queue = queues.at(i);
		if ( (deleted && queue->getStatus()==Queue::DELETED) ||
				!(deleted || queue->getStatus()==Queue::DELETED) )
			selected.append(queue);
	}

	return selected;
}


/*
 * Search for a queue with a given name.
 * Parameter 'deleted' indicates whether search will be done for existing
 * or deleted (during the time application was running) queues.
 * By default 'deleted' is set to false, and search goes only through existing queues.
 */
Queue* DelilahQtApp::getQueue(const QString &name, bool deleted)
{
	for(int i=0; i<queues.size(); i++)
	{
		Queue* queue = queues.at(i);
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
 * Synchronizes key-value queues list with the queues returned in HelpResponse.
 * If synchronize_all is true, all queues that were not updated (have status SYNCHRONIZING)
 * are marked as deleted.
 */
void DelilahQtApp::synchronizeQueues(const ss::network::HelpResponse &resp, bool synchronize_all)
{
	for (int i=0 ; i<resp.queue_size(); i++)
	{
		ss::network::Queue q = resp.queue(i);
		QString name = QString::fromStdString(q.name());

		// find if queue with such name already exists. If not, create
		// the new one and add it to the global list.
		Queue *queue = getQueue(name);
		if (queue==0)
		{
			queue = new Queue(name);
			queues.append(queue);
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
		for (int i=0; i<queues.size(); i++)
		{
			if ( queues.at(i)->getStatus() == Queue::SYNCHRONIZING )
				queues.at(i)->setStatus(Queue::DELETED);
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
