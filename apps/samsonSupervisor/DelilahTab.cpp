/* ****************************************************************************
*
* FILE                     DelilahTab.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 02 2011
*
*/
#include <QWidget>
#include <QGridLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>

#include "logMsg.h"             // LM_*
#include "globals.h"            // networkP

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

	input->clear();
	output->clear();
	output->append(QString(command) + ":");
	output->append(QString(" [ Command '") + QString(command) + QString("' sent. Awaiting completion ... ]"));

	delilahConsole->writeCallbackSet(consoleOut);
	delilahConsole->evalCommand(commandStdString);
}
