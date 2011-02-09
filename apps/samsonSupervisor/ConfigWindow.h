#ifndef CONFIG_WINDOW_H
#define CONFIG_WINDOW_H

/* ****************************************************************************
*
* FILE                     ConfigWindow.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jan 25 2011
*
*/
#include <QObject>
#include <QDialog>
#include <QCheckBox>
#include <QPushButton>
#include <QListWidgetItem>

#include "Endpoint.h"           // Endpoint



/* ****************************************************************************
*
* TRACE_LEVELS
*/
#define TRACE_LEVELS     256



/* ****************************************************************************
*
* ConfigWindow - 
*/
class ConfigWindow : public QDialog
{
	Q_OBJECT

public:
	ConfigWindow(ss::Endpoint* endpoint);

private:
	bool                      allTraceLevelsState;
	ss::Endpoint*             endpoint;

	QCheckBox*                verboseBox;
	QCheckBox*                debugBox;
	QCheckBox*                readsBox;
	QCheckBox*                writesBox;
	QCheckBox*                toDoBox;
	QCheckBox*                logBox;
	QPushButton*              allTraceLevelsItem;
	QListWidgetItem*          traceLevelItem[TRACE_LEVELS];

private slots:
	void send(void);
	void all();
};

#endif
