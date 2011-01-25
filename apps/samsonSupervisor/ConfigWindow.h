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
#include <QListWidgetItem>

#include "Endpoint.h"           // Endpoint



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
	ss::Endpoint*             endpoint;

	QCheckBox*                verboseBox;
	QCheckBox*                debugBox;
	QCheckBox*                readsBox;
	QCheckBox*                writesBox;
	QListWidgetItem*          traceLevelItem[256];

private slots:
	void send(void);
};

#endif
