/* ****************************************************************************
*
* FILE                     QueueConfigWindow.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 04 2011
*
*/
#include <QApplication>
#include <QDialog>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QSize>
#include <QDesktopWidget>

#include "logMsg.h"             // LM_X, ...
#include "traceLevels.h"        // Trace Levels
#include "globals.h"            // qtAppRunning, ...

#include "DelilahQueue.h"       // DelilahQueue
#include "QueueConfigWindow.h"  // Own interface



/* ****************************************************************************
*
* QueueConfigWindow::QueueConfigWindow - 
*/
QueueConfigWindow::QueueConfigWindow(DelilahQueue* queue)
{
	QGridLayout*      layout;

	char              textV[128];
	QLabel*           title;
	QFont             titleFont("Times", 20, QFont::Normal);

	QLabel*           displayNameLabel;
	QLabel*           inTypeLabel;
	QLabel*           outTypeLabel;

	QDialogButtonBox* buttonBox;

	QLabel*           realNameLabel;
	QLabel*           noOfOutgoingLabel;
	QLabel*           noOfIncomingLabel;

	QSize             size;
	int               screenWidth;
	int               screenHeight;
	int               x;
	int               y;
	QDesktopWidget*   desktop = QApplication::desktop();

	int               incoming;
	int               outgoing;

	this->queue = queue;

	setModal(true);

	layout = new QGridLayout();

	snprintf(textV, sizeof(textV), "Configuring Queue '%s'", queue->displayName);
	title = new QLabel(textV);
	title->setFont(titleFont);
	
	displayNameLabel = new QLabel("Display Name");
	displayNameInput = new QLineEdit();
	displayNameInput->setText(queue->displayName);

	inTypeLabel = new QLabel("Incoming Type");
	inTypeInput = new QLineEdit();
	inTypeInput->setText(queue->inType);

	outTypeLabel = new QLabel("Outgoing Type");
	outTypeInput = new QLineEdit();
	outTypeInput->setText(queue->outType);

	incoming = connectionMgr->incomingConnections(queue);
	snprintf(textV, sizeof(textV), "%d Incoming connections", incoming);
	noOfIncomingLabel = new QLabel(textV);

	outgoing = connectionMgr->outgoingConnections(queue);
	snprintf(textV, sizeof(textV), "%d Outgoing connections", outgoing);
	noOfOutgoingLabel = new QLabel(textV);
	
	snprintf(textV, sizeof(textV), "Platform Queue name: '%s'", queue->name);
	realNameLabel = new QLabel(textV);

	QPushButton* okButton     = new QPushButton("OK");
	QPushButton* saveButton   = new QPushButton("Save");
	QPushButton* cancelButton = new QPushButton("Cancel");
	QPushButton* applyButton  = new QPushButton("Apply");

	buttonBox = new QDialogButtonBox();
	buttonBox->addButton(okButton,     QDialogButtonBox::AcceptRole);
	buttonBox->addButton(saveButton,   QDialogButtonBox::ActionRole);
	buttonBox->addButton(applyButton,  QDialogButtonBox::ActionRole);
	buttonBox->addButton(cancelButton, QDialogButtonBox::RejectRole);

	connect(okButton,     SIGNAL(clicked()), this, SLOT(ok()));
	connect(saveButton,   SIGNAL(clicked()), this, SLOT(save()));
	connect(applyButton,  SIGNAL(clicked()), this, SLOT(apply()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));

	setWindowTitle("Samson Queue Configuration");

	layout->addWidget(title,             0, 0, 1, 2);
	layout->addWidget(displayNameLabel,  1, 0);
	layout->addWidget(displayNameInput,  1, 1);
	layout->addWidget(inTypeLabel,       2, 0);
	layout->addWidget(inTypeInput,       2, 1);
	layout->addWidget(outTypeLabel,      3, 0);
	layout->addWidget(outTypeInput,      3, 1);
	layout->addWidget(noOfIncomingLabel, 5, 0);
	layout->addWidget(noOfOutgoingLabel, 6, 0);
	layout->addWidget(realNameLabel,     7, 0);
	layout->addWidget(buttonBox,         9, 0, 1, 2);

	this->setLayout(layout);
	this->show();

	// Window Geometry
	size = this->size();

	screenWidth  = desktop->width();
	screenHeight = desktop->height();

	x = (screenWidth  - size.width())  / 2;
	y = (screenHeight - size.height()) / 2;

	this->move(x, y);

	if (qtAppRunning == false)
	{
		qtAppRunning = true;
		qApp->exec();
	}
}



/* ****************************************************************************
*
* die
*/
void QueueConfigWindow::die(void)
{
	LM_X(1, ("Dying popup received OK press event"));
}



/* ****************************************************************************
*
* ok
*/
void QueueConfigWindow::ok(void)
{
	LM_T(LmtMouseEvent, ("OK pressed in Queue Config Dialog"));
	
	save();
	delete this;
}



/* ****************************************************************************
*
* save
*/
void QueueConfigWindow::save(void)
{
	char* cP;

	LM_T(LmtMouseEvent, ("SAVE pressed in Queue Config Dialog"));

	cP = (char*) displayNameInput->text().toStdString().c_str();
	if (cP != NULL)
		queue->displayNameSet(cP);

	cP = (char*) inTypeInput->text().toStdString().c_str();
    if (cP != NULL)
		queue->inTypeSet(cP);

	cP = (char*) outTypeInput->text().toStdString().c_str();
    if (cP != NULL)
		queue->outTypeSet(cP);
}



/* ****************************************************************************
*
* apply
*/
void QueueConfigWindow::apply(void)
{
	LM_T(LmtMouseEvent, ("APPLY pressed in Queue Config Dialog"));
	save();
}



/* ****************************************************************************
*
* cancel
*/
void QueueConfigWindow::cancel(void)
{
	LM_T(LmtMouseEvent, ("CANCEL pressed in Queue Config Dialog"));
	delete this;
}
