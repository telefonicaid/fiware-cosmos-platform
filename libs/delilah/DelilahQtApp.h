/*
 * DelilahQtApp.h
 *
 *  Created on: Nov 4, 2010
 *      Author: ania
 */

#ifndef DELILAHQTAPP_H_
#define DELILAHQTAPP_H_

#include <QApplication>

class MainWindow;
namespace ss{
	class Delilah;
}

class DelilahQtApp : public QApplication
{
	Q_OBJECT

public:
	DelilahQtApp(int &argc, char ** argv, ss::Delilah* delilah);
	~DelilahQtApp() {};

	QString validateNewQueueName(QString name);
	size_t sendCreateQueue(const QString &name);
	size_t sendCreateQueue(const QString &name, const QString &key_type, const QString &value_type);

	void receivedMessage(size_t id, bool error, bool finished, std::string message);

public slots:
	void quitDelilah();

protected:
	size_t sendMessage(QString _command);

signals:
	void jobUpdated(size_t id, bool error, QString message);
	void jobFinished(size_t id, bool error, QString message);

public:
	MainWindow* w;

private:
	ss::Delilah* client;
};


#endif /* DELILAHQTAPP_H_*/
