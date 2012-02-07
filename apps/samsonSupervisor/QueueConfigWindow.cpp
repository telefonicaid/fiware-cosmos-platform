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
#include <QComboBox>

#include "logMsg.h"             // LM_X, ...
#include "traceLevels.h"        // Trace Levels
#include "globals.h"            // delilahConsole

#include "DelilahQueue.h"       // DelilahQueue
#include "QueueConfigWindow.h"  // Own interface



/* ****************************************************************************
*
* win - 
*/
static QueueConfigWindow* win = NULL;



/* ****************************************************************************
*
* imageV - stores absolute path to images
*/
static char imageV[256];



/* ****************************************************************************
*
* QueueConfigWindow::QueueConfigWindow - 
*/
QueueConfigWindow::QueueConfigWindow(DelilahQueue* queue)
{
	QGridLayout*      layout;
    QLabel*           imageLabel;
    QPixmap*          image;

	char              textV[128];
	QLabel*           title;
	QFont             titleFont("Times", 20, QFont::Normal);

	QLabel*           displayNameLabel;
	QLabel*           inTypeLabel;
	QLabel*           outTypeLabel;
	QLabel*           commandLabel;

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

	win = this;

	this->queue = queue;

	setModal(true);

	layout = new QGridLayout();

	image      = new QPixmap(imagePath("queue.png", imageV, sizeof(imageV)));
	imageLabel = new QLabel();
	imageLabel->setPixmap(*image);

	snprintf(textV, sizeof(textV), "Configuring Queue '%s'", queue->displayName);
	title = new QLabel(textV);
	title->setFont(titleFont);
	
	displayNameLabel = new QLabel("Display Name");
	displayNameInput = new QLineEdit();
	displayNameInput->setText(queue->displayName);

	inTypeLabel = new QLabel("Incoming Type");
	inTypeCombo = new QComboBox();

	outTypeLabel = new QLabel("Outgoing Type");
	outTypeCombo = new QComboBox();

	commandLabel = new QLabel("Command");
	commandCombo = new QComboBox();

    //delilahConsole->writeCallbackSet(dataTypesTextReceiver);
    //delilahConsole->evalCommand("datas");
	
	incoming = connectionMgr->incomingConnections(queue);
	snprintf(textV, sizeof(textV), "%d Incoming connections", incoming);
	noOfIncomingLabel = new QLabel(textV);

	outgoing = connectionMgr->outgoingConnections(queue, NULL);
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

    layout->addWidget(imageLabel,                 0, 0, 9, 1);
	layout->addWidget(title,                      0, 1, 1, 2);
	layout->addWidget(displayNameLabel,           1, 1);
	layout->addWidget(displayNameInput,           1, 2);
	layout->addWidget(inTypeLabel,                2, 1);
	layout->addWidget(inTypeCombo,                2, 2);
	layout->addWidget(outTypeLabel,               3, 1);
	layout->addWidget(outTypeCombo,               3, 2);
	layout->addWidget(commandLabel,               4, 1);
	layout->addWidget(commandCombo,               4, 2);
	layout->addWidget(noOfIncomingLabel,          5, 1);
	layout->addWidget(noOfOutgoingLabel,          6, 1);
	layout->addWidget(realNameLabel,              7, 1);
	layout->addWidget(buttonBox,                  9, 1, 1, 2);

	this->setLayout(layout);
	this->show();

	// Window Geometry
	size = this->size();

	screenWidth  = desktop->width();
	screenHeight = desktop->height();

	x = (screenWidth  - size.width())  / 2;
	y = (screenHeight - size.height()) / 2;

	this->move(x, y);
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

	queue->inTypeSet(inTypeCombo->currentText().toStdString().c_str(), inTypeCombo->currentIndex());
	queue->outTypeSet(outTypeCombo->currentText().toStdString().c_str(), outTypeCombo->currentIndex());
	queue->commandSet(commandCombo->currentText().toStdString().c_str(), commandCombo->currentIndex());

	LM_T(LmtQueue, ("Set inType  to '%s' (current index: %d)", queue->inType,  win->queue->inTypeIndex));
	LM_T(LmtQueue, ("Set outType to '%s' (current index: %d)", queue->outType, win->queue->outTypeIndex));
	LM_T(LmtQueue, ("Set command to '%s' (current index: %d)", queue->command, win->queue->commandIndex));
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