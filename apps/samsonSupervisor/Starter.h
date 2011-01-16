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

#include "Endpoint.h"           // Endpoint
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

	Spawner*        spawner;
	Process*        process;

	QCheckBox*      checkbox;
	ss::Endpoint*   endpoint;

	char*           type;
	char*           name;

private slots:
	void spawnerClicked();
	void processClicked();

private:
	Qt::CheckState  checkState;

	bool            connected;
};

#endif
