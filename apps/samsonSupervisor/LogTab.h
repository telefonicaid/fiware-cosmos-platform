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
#include <QObject>
#include <QWidget>



/* ****************************************************************************
*
* LogTab -
*/
class LogTab : public QWidget
{
	Q_OBJECT

public:
	LogTab(QWidget *parent = 0);
};

#endif
