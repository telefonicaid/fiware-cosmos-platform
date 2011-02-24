#ifndef LOG_FILE_WINDOW_H
#define LOG_FILE_WINDOW_H

/* ****************************************************************************
*
* FILE                     LogFileWindow.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 24 2011
*
*/
#include <QObject>
#include <QDialog>

#include "Endpoint.h"           // ss::Endpoint



/* ****************************************************************************
*
* LogFileWindow - 
*/
class LogFileWindow : public QDialog
{
	Q_OBJECT

public:
	LogFileWindow(ss::Endpoint* ep, char* text, bool old = false);
};

#endif
