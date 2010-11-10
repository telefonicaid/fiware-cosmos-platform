/*
 * DelilahQtApp.cpp
 *
 *  Created on: Nov 4, 2010
 *      Author: ania
 */

#include "DelilahQtApp.h"
#include "MainWindow.h"
#include "Delilah.h"

DelilahQtApp::DelilahQtApp(int &argc, char ** argv, ss::Delilah* delilah)
	: QApplication(argc, argv)
{
	client = delilah;
	w = new MainWindow();			// My main window interface
	w->show();

	connect(this, SIGNAL(lastWindowClosed()), this, SLOT(quitDelilah()));
}

void DelilahQtApp::quitDelilah()
{
	client->quit();
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

size_t DelilahQtApp::sendCreateQueue(const QString &name)
{
	// TODO:
//	QString command = QString("add_queue %1").arg(name);
//	return sendMessage(command);
	// For testing
	std::cout << "SENDING request\n";
	return 1;
}

size_t DelilahQtApp::sendCreateQueue(const QString &name, const QString &key_type, const QString &value_type)
{
	QString command = QString("add_queue %1 %2 %3").arg(name).arg(key_type).arg(value_type);
	return sendMessage(command);
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

