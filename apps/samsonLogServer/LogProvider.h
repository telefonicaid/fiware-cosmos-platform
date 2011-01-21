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



/* ****************************************************************************
*
* LogProvider - 
*/
class LogProvider : public QObject
{
	Q_OBJECT;

public:
	LogProvider(const char* name, const char* host, int fd);
	~LogProvider();
	void connectionClosed(void);

public:
	char*         name;
	char*         host;
	int           fd;
	bool          folded;

	QHBoxLayout*  headerLayout;

	QPushButton*  foldButton;
	QLabel*       stateLabel;
	QLabel*       nameLabel;
	QPushButton*  clearButton;
	QPushButton*  removeButton;

	QListWidget*  list;

	QImage*       delimiter;

public slots:
	void fold();
	void clear();
	void remove();
};

#endif
