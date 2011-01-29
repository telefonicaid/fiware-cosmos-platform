#ifndef STARTER_H
#define STARTER_H

/* ****************************************************************************
*
* FILE                     Starter.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jan 13 2011
*
*/
#include <QObject>
#include <QBoxLayout>
#include <QCheckBox>
#include <QPushButton>
#include <QGridLayout>

#include "Process.h"            // Process



/* ****************************************************************************
*
* Starter - 
*/
class Starter : public QWidget
{
	Q_OBJECT

public:
	Starter(Process* processP);
	Starter(Process* processP, SpawnInfo* siP);
	
	void            check(void);
	void            qtInit(QGridLayout* grid, int row, int column);

	Process*        process;

	Qt::CheckState  checkState;
	QCheckBox*      checkbox;
	QPushButton*    configButton;

private slots:
	void spawnerClicked();
	void processClicked();
	void configureClicked();
};

#endif
