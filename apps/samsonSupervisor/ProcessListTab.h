#ifndef PROCESS_LIST_TAB_H
#define PROCESS_LIST_TAB_H

/* ****************************************************************************
*
* FILE                     ProcessListTab.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jan 14 2011
*
*/
#include <QObject>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "Endpoint.h"           // Endpoint
#include "Starter.h"            // Starter



/* ****************************************************************************
*
* ProcessListTab - 
*/
class ProcessListTab : public QWidget
{
	Q_OBJECT

public:
	ProcessListTab(const char* name, QWidget *parent = 0);
	void         starterInclude(Starter* starterP);

private slots:
	void quit();

private:
	QGridLayout*  mainLayout;
	void          initialStartersCreate(void);
};

#endif
