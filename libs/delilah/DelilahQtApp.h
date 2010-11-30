/*
 * DelilahQtApp.h
 *
 *  Created on: Nov 4, 2010
 *      Author: ania
 */

#ifndef DELILAHQTAPP_H_
#define DELILAHQTAPP_H_

#include <QApplication>

#include "Message.h"

#include "Queue.h"
class MainWindow;
class Operation;
class DataType;
namespace ss {
	class Delilah;
	class Packet;
	namespace network {
		class Queue;
		class HelpResponse;
	}
}

class DelilahQtApp : public QApplication
{
	Q_OBJECT

public:
	DelilahQtApp(int &argc, char ** argv, ss::Delilah* _delilah);
	~DelilahQtApp() {};

	QString validateNewQueueName(QString name);

	QList<Queue*> getQueues(bool deleted=false);
	Queue* getQueue(const QString &name, bool deleted=false);
	QList<DataType*> getDataTypes();
	DataType* getDataType(const QString &name);
	QList<Operation*> getOperations();
	Operation* getOperation(const QString &name);

	/*
	 * Methods sending requests to network
	 */
	void uploadData(bool queue=true, bool operation=true, bool data_type=true, const QString &name="");
	int sendCreateQueue(const QString &name);
	int sendCreateQueue(const QString &name, const QString &key_type, const QString &value_type);
	int sendDeleteQueue(const QString &name);

	/*
	 * Methods receiving packets from network
	 */
	int receiveData(ss::Packet* packet);
	int receiveCommandResponse(ss::Packet* packet);
	int receiveUknownPacket(size_t id, ss::Message::MessageCode msgCode, ss::Packet* packet);

public slots:
	void quitDelilah();

protected:
	/*
	 * Methods synchronizing application's lists with information received from network.
	 */
	void synchronizeQueues(const ss::network::HelpResponse &resp, bool synchronize_all=true);
	void synchronizeOperations(const ss::network::HelpResponse &resp, bool synchronize_all=true);
	void synchronizeDataTypes(const ss::network::HelpResponse &resp, bool synchronize_all=true);

	int sendCommand(std::string);		// Sends command to the network

signals:
	void gotCommandResponse(unsigned int id, bool finished, bool error, QString message);

public:
	MainWindow* w;							// Main Window of application

protected:
	ss::Delilah* delilah;					// Pointer to the most upper class of SAMSON client application
	unsigned int id;						// Counter of requests sent to the network
											// (initialized with value set to 0).

	// Lists of queues, operations and data types currently available in the system.
	// They are automatically uploaded on application startup.
	// The operations and data types can not be changed runtime. The SAMSON platform
	// has to be restarted to upload new operations/data types, and so the DelilahQt application.
	// Queues can be dynamically created by user.
	// The problem of updating data_queues and kv_queues is not solved (TODO)!!!!!!!!!!!
	// The best solution would be to get some signal from SAMSON platform when new queue is added
	// and update the list. However this mechanism does not exist.
	// Current solution:
	// 1. When user creates/deletes a queue, the queue is also added/removed to/from the list.
	// 2. Application contains button/menu where user can click to manually upload queues from
	// the platform (TODO).
	QList<Queue*> queues;
	QList<Operation*> operations;
	QList<DataType*> data_types;
};


#endif /* DELILAHQTAPP_H_*/
