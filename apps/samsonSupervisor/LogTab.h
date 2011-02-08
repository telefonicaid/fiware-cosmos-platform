#ifndef LOG_TAB_H
#define LOG_TAB_H

/* ****************************************************************************
*
* FILE                     LogTab.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 02 2011
*
*/
#include <netinet/in.h>         // sockaddr_in

#include <QObject>
#include <QWidget>

#include "Message.h"            // ss::Message::Header, ss::Message::LogLineData



class QTableWidget;



/* ****************************************************************************
*
* LogTab -
*/
class LogTab : public QWidget
{
	Q_OBJECT

public:
	LogTab(QWidget *parent = 0);

	void logItemAdd(int row, const char  typ, const char* date, const char* host, const char* pName, const char* file, int lineNo, const char* fName, const char* text, int tLevel = 0);
	void logLineInsert(struct sockaddr_in* sAddr, ss::Message::Header* headerP, ss::Message::LogLineData* logLine);

	QTableWidget*  tableWidget;
	int            Rows;
};

#endif
