#ifndef LOG_PROVIDER_H
#define LOG_PROVIDER_H

/* ****************************************************************************
*
* FILE                     LogProvider.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jan 20 2011
*
*/
#include <QListWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>

#include "Endpoint.h"           // ss::Endpoint



/* ****************************************************************************
*
* LogProvider - 
*/
class LogProvider : public QObject
{
	Q_OBJECT;

public:
	LogProvider(ss::Endpoint* ep, const char* name, const char* host, int fd);
	~LogProvider();
	void connectionClosed(void);

public:
	char*         name;
	char*         host;
	int           fd;
	bool          folded;
	ss::Endpoint* endpoint;

	QHBoxLayout*  headerLayout;

	QPushButton*  foldButton;
	QLabel*       stateLabel;
	QLabel*       nameLabel;
	QPushButton*  clearButton;
	QPushButton*  pauseButton;
	QPushButton*  stopButton;
	QPushButton*  removeButton;

	QListWidget*  list;

	QImage*       delimiter;

public slots:
	void fold();
	void clear();
	void pause();
	void stop();
	void remove();
};

#endif
