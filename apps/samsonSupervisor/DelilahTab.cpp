#include <QWidget>
#include <QGridLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>

#include "logMsg.h"             // LM_*
#include "globals.h"            // networkP
#include "SamsonSetup.h"		// ss::SamsonSetup
#include "MemoryManager.h"      // ss::MemoryManager
#include "Delilah.h"            // ss::Delilah
#include "DelilahConsole.h"     // ss::DelilahConsole

#include "DelilahTab.h"         // Own interface



/* ****************************************************************************
*
* consoleOut - 
*/
void consoleOut(const char* type, const char* text)
{
	DelilahTab* dTab = tabManager->delilahTab;

	dTab->output->clear();
	dTab->output->append(QString(type) + ":");
	dTab->output->append(QString(text));
}



/* ****************************************************************************
*
* DelilahTab::DelilahTab - 
*/
DelilahTab::DelilahTab(const char* name, QWidget *parent) : QWidget(parent)
{
	int memory_gb           =  1;
	int load_buffer_size_mb = 64;

	ss::SamsonSetup::load();
	ss::SamsonSetup::shared()->memory           = (size_t) memory_gb * (size_t) (1024*1024*1024);
	ss::SamsonSetup::shared()->load_buffer_size = (size_t) load_buffer_size_mb * (size_t) (1024*1024);
	ss::MemoryManager::init();


	delilah        = new ss::Delilah(networkP);
	delilahConsole = new ss::DelilahConsole(delilah);

	delilahConsole->writeCallbackSet(consoleOut);

	mainLayout  = new QGridLayout();
	input       = new QLineEdit();
	sendButton  = new QPushButton("Send");
	output      = new QTextEdit("Output Window");

	mainLayout->addWidget(input, 0, 0, 1, 4);
	mainLayout->addWidget(sendButton, 0, 4);
	mainLayout->addWidget(output, 1, 0);

	sendButton->connect(sendButton, SIGNAL(clicked()), this, SLOT(send()));
	output->setReadOnly(true);

	setLayout(mainLayout);
}



/* ****************************************************************************
*
* DelilahTab::send - 
*/
void DelilahTab::send(void)
{
	QString        commandQString   = input->displayText();
	std::string    commandStdString = commandQString.toStdString();
	const char*    command          = commandStdString.c_str();

	LM_M(("Sending delilah command '%s'", command));

	input->clear();
	output->clear();
	output->append(QString(command) + ":");
	output->append(QString(" [ Command '") + QString(command) + QString("' sent. Awaiting completion ... ]"));

	delilahConsole->evalCommand(commandStdString);
}
