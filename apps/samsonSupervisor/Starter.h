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

#include "Spawner.h"            // Spawner
#include "Process.h"            // Process



/* ****************************************************************************
*
* Starter - 
*/
class Starter : public QWidget
{
	Q_OBJECT

public:
	Starter(const char* type, char* name, bool initialState = true);
	void spawnerSet(Spawner* s);
	void processSet(Process* s);

	QCheckBox*      checkbox;

private slots:
	void spawnerClicked();
	void processClicked();

private:
	Qt::CheckState  checkState;

	Spawner*        spawner;
	Process*        process;
	bool            connected;
};

#endif
