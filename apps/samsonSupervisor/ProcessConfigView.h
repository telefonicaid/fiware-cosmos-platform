#ifndef PROCESS_CONFIG_VIEW
#define PROCESS_CONFIG_VIEW

/* ****************************************************************************
*
* FILE                     ProcessConfigView.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 09 2011
*
*/
#include <QObject>
#include <QWidget>

class QGridLayout;
class QCheckBox;
class QPushButton;
class QListWidgetItem;
class QListWidget;
class QLabel;

#include "Process.h"            // Process



/* ****************************************************************************
*
* TRACE_LEVELS
*/
#define TRACE_LEVELS     256



/* ****************************************************************************
*
* ProcessConfigView - 
*/
class ProcessConfigView : public QWidget
{
	Q_OBJECT

public:
	ProcessConfigView(QGridLayout* grid, Process* process);
	~ProcessConfigView();

private:
	Process*          process;
    bool              allTraceLevelsState;

	QGridLayout*      grid;
	QLabel*           label;
	QPushButton*      sendButton;
	QLabel*           traceLevelLabel;
	QListWidget*      traceLevelList;
    QCheckBox*        verboseBox;
    QCheckBox*        debugBox;
    QCheckBox*        readsBox;
    QCheckBox*        writesBox;
    QCheckBox*        toDoBox;
    QPushButton*      allTraceLevelsItem;
    QListWidgetItem*  traceLevelItem[TRACE_LEVELS];

private slots:
	void send(void);
	void all();
};

#endif
