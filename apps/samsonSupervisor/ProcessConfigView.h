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
class QLineEdit;
class QVBoxLayout;

#include "Process.h"            // ss::Process
#include "Worker.h"             // ss::Worker
#include "Message.h"            // ss::Message



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
	ProcessConfigView(QGridLayout* grid, ss::Process* process, ss::Message::ConfigData* configData);
	ProcessConfigView(QGridLayout* grid, ss::Process* process, ss::Worker*              workerP);
	ProcessConfigView(QGridLayout* grid, ss::Process* process);
	~ProcessConfigView();

	void init(QGridLayout* grid, ss::Process* process);
	void fill(QGridLayout* grid, ss::Process* process);
	void processSave(void);

	ss::Process*      process;
	bool              hostEditable;

private:
    bool              allTraceLevelsState;

	QGridLayout*      grid;
	QLabel*           label;
	QPushButton*      sButton;
	QLabel*           traceLevelLabel;
	QListWidget*      traceLevelList;
    QCheckBox*        verboseBox;
    QCheckBox*        debugBox;
    QCheckBox*        readsBox;
    QCheckBox*        writesBox;
    QCheckBox*        toDoBox;
    QPushButton*      allTraceLevelsItem;
	QListWidgetItem*  traceLevelItem[TRACE_LEVELS];
	QLabel*           aliasLabel;
	QLineEdit*        aliasEdit;
	QLabel*           hostLabel;
	QLineEdit*        hostEdit;
	QVBoxLayout*      ahLayout;

private slots:
	void send(void);
	void save(void);
	void all();
};

#endif
