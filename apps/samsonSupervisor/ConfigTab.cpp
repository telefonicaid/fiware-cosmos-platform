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
#include <QLabel>
#include <QSlider>

#include "logMsg.h"             // LM_*

#include "ConfigTab.h"          // Own interface



/* ****************************************************************************
*
* ConfigTab::ConfigTab - 
*/
ConfigTab::ConfigTab(QWidget *parent) : QWidget(parent)
{
	QGridLayout*  mainLayout        = new QGridLayout(parent);
	QGroupBox*    connectBox        = new QGroupBox("Connection Type Checking");
	QVBoxLayout*  vbox              = new QVBoxLayout;
	QRadioButton* onButton          = new QRadioButton("Error on different types");
	QRadioButton* offButton         = new QRadioButton("Automatic type checking OFF");
	QRadioButton* leftButton        = new QRadioButton("Inherit left side out-type");
	QRadioButton* popupButton       = new QRadioButton("Popup to choose type");
	QLabel*       logFontSizeLabel  = new QLabel("Font size in Log tab");
	QSlider*      logFontSizeSlider = new QSlider(Qt::Horizontal);

	autoConfigCBox = new QCheckBox("Config window on SceneItem creation");
	autoConfigCBox->setCheckState(Qt::Unchecked);

	setLayout(mainLayout);
	mainLayout->addWidget(connectBox,           0, 0, 5, 1);
	mainLayout->addWidget(autoConfigCBox,       1, 1, 1, 1);

	mainLayout->addWidget(logFontSizeLabel,     2, 1, 1, 1);
	mainLayout->addWidget(logFontSizeSlider,    2, 2, 1, 1);

	onButton->setChecked(true);

	vbox->addWidget(onButton);
	vbox->addWidget(offButton);
	vbox->addWidget(leftButton);
	vbox->addWidget(popupButton);
	vbox->addStretch(1);

	connect(onButton,    SIGNAL(clicked()), this, SLOT(typeCheckOn()));
	connect(offButton,   SIGNAL(clicked()), this, SLOT(typeCheckOff()));
	connect(leftButton,  SIGNAL(clicked()), this, SLOT(typeCheckLeft()));
	connect(popupButton, SIGNAL(clicked()), this, SLOT(typeCheckPopup()));
	connectBox->setLayout(vbox);
	connectBox->setFlat(false);
	connectBox->setFlat(false);
	connectBox->setFlat(false);

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
