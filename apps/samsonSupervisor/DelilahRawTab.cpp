/* ****************************************************************************
*
* FILE                     DelilahRawTab.cpp
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

#include "DelilahRawTab.h"      // Own interface



/* ****************************************************************************
*
* consoleOut - 
*/
void consoleOut(const char* type, const char* text)
{
	DelilahRawTab* dTab = tabManager->delilahRawTab;

	dTab->output->clear();
	dTab->output->append(QString(type) + ":");
	dTab->output->append(QString(text));
}



/* ****************************************************************************
*
* DelilahRawTab::DelilahRawTab - 
*/
DelilahRawTab::DelilahRawTab(const char* name, QWidget *parent) : QWidget(parent)
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
	connect(input, SIGNAL(returnPressed()), this, SLOT(send()));

	setLayout(mainLayout);
}



/* ****************************************************************************
*
* DelilahRawTab::send - 
*/
void DelilahRawTab::send(void)
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
