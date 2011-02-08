/* ****************************************************************************
*
* FILE                     ConfigTab.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 02 2011
*
*/
#include <QWidget>
#include <QGridLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QCheckBox>

#include "logMsg.h"             // LM_*

#include "ConfigTab.h"          // Own interface



/* ****************************************************************************
*
* ConfigTab::ConfigTab - 
*/
ConfigTab::ConfigTab(QWidget *parent) : QWidget(parent)
{
	QGridLayout*  mainLayout       = new QGridLayout(parent);
	QGroupBox*    connectBox       = new QGroupBox("Connection Type Checking");
	QVBoxLayout*  vbox             = new QVBoxLayout;
	QRadioButton* onButton         = new QRadioButton("Error on different types");
	QRadioButton* offButton        = new QRadioButton("Automatic type checking OFF");
	QRadioButton* leftButton       = new QRadioButton("Inherit left side out-type");
	QRadioButton* popupButton      = new QRadioButton("Popup to choose type");

	autoConfigCBox   = new QCheckBox("Config window on SceneItem creation");
	autoConfigCBox->setCheckState(Qt::Unchecked);

	setLayout(mainLayout);
	mainLayout->addWidget(connectBox, 0, 0, 5, 1);
	mainLayout->addWidget(autoConfigCBox, 0, 1, 1, 1);

	connectBox->setFlat(false);
	onButton->setChecked(true);

	vbox->addWidget(onButton);
	vbox->addWidget(offButton);
	vbox->addWidget(leftButton);
	vbox->addWidget(popupButton);
	vbox->addStretch(1);

	connect(onButton,    SIGNAL(clicked()), this, SLOT(typeCheckOn()));
	connect(offButton,   SIGNAL(clicked()), this, SLOT(typeCheckOf()));
	connect(leftButton,  SIGNAL(clicked()), this, SLOT(typeCheckLeft()));
	connect(popupButton, SIGNAL(clicked()), this, SLOT(typeCheckPopup()));
	connectBox->setLayout(vbox);

	typeCheck = On;
}



/* ****************************************************************************
*
* ConfigTab::typeCheckOn - 
*/
void ConfigTab::typeCheckOn(void)
{
	LM_W(("Type Check ON pressed"));
	typeCheck = On;
}



/* ****************************************************************************
*
* ConfigTab::typeCheckOff - 
*/
void ConfigTab::typeCheckOff(void)
{
	LM_W(("Type Check OFF pressed"));
	typeCheck = Off;
}



/* ****************************************************************************
*
* ConfigTab::typeCheckLeft - 
*/
void ConfigTab::typeCheckLeft(void)
{
	LM_W(("Type Check LEFT pressed"));
	typeCheck = Left;
}



/* ****************************************************************************
*
* ConfigTab::typeCheckPopup - 
*/
void ConfigTab::typeCheckPopup(void)
{
	LM_W(("Type Check POPUP pressed"));
	typeCheck = Popup;
}
