#ifndef PROCESS_LIST_TAB_H
#define PROCESS_LIST_TAB_H

#include <QObject>
#include <QWidget>



/* ****************************************************************************
*
* ProcessListTab - 
*/
class ProcessListTab : public QWidget
{
	Q_OBJECT

public:
	ProcessListTab(const char* name, QWidget *parent = 0);
};

#endif
