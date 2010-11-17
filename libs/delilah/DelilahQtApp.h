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
//class DataQueue;
class KVQueue;
class Operation;
class DataType;
namespace ss {
	class Delilah;
	class Packet;
	namespace network {
		class Queue;
		class DataQueue;
	}
}

class DelilahQtApp : public QApplication
{
	Q_OBJECT

public:
	DelilahQtApp(int &argc, char ** argv, ss::Delilah* _delilah);
	~DelilahQtApp() {};

	QString validateNewQueueName(QString name);

	DataQueue* getDataQueue(const QString &name);
	KVQueue* getKVQueue(const QString &name);

	/*
	 * Methods sending requests to network
	 */
	void uploadData(bool queues=true, bool operations=true, bool data_types=true, const QString &name="");
	int sendCreateDataQueue(const QString &name);
	int sendCreateKVQueue(const QString &name, const QString &key_type, const QString &value_type);

	/*
	 * Methods receiving packets from network
	 */
	int receiveData(ss::Packet* packet);
	int receiveCommandResponse(ss::Packet* packet);
	int receiveUknownPacket(size_t id, ss::Message::MessageCode msgCode, ss::Packet* packet);

public slots:
	void quitDelilah();

signals:
	void gotCommandResponse(unsigned int id, bool finished, bool error, QString message);

public:
	MainWindow* w;							// Main Window of application

protected:
	ss::Delilah* delilah;					// Pointer to the most upper class of SAMSON client application
	unsigned int id;						// Counter of requests sent to the network
											// (initialized with value set to 0).

	// Lists of (data) queues, operations and data types currently available in the system.
	// They are automatically uploaded (TODO) on application startup.
	// Currently, the operations and data types can not be dynamically changed. The SAMSON platform
	// has to be restarted to upload new operations/data types, and so the DelilahQt application.
	// Queues can be dynamically created by user.
	// The problem of updating data_queues and kv_queues is not solved (TODO)!!!!!!!!!!!
	// The best solution would be to get some signal from SAMSON platform when new queue is added
	// and update the list. However this mechanism does not exist.
	// Current solution:
	// 1. When user creates/deletes a queue, the queue is also added/removed to/from the list (TODO).
	// 2. Application contains button/menu where user can click to manually upload queues from
	// the platform (TODO).
	QList<DataQueue*> data_queues;
	QList<KVQueue*> kv_queues;
	QList<Operation*> operations;
	QList<DataType*> data_types;
};


#endif /* DELILAHQTAPP_H_*/
