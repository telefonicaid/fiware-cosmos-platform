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
	enum Type
	{
		ProcessStarter,
		SpawnerConnecter
	};

public:
	Starter(Spawner* spawnerP);
	Starter(Process* processP);
	void init(const char* name, Type type);

	const char*     typeName(void);
	void            forceCheck(void);
	void            forceUncheck(void);
	void            check(void);
	bool            checked(void);

	Spawner*        spawner;
	Process*        process;

	ss::Endpoint*   endpoint;

	Type            type;
	char*           name;
	Qt::CheckState  checkState;
	bool            connected;

	QCheckBox*      checkbox;

private slots:
	void spawnerClicked();
	void processClicked();
};

#endif
