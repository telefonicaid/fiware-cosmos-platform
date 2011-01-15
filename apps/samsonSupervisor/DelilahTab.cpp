#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

#include "logMsg.h"             // LM_*

#include "DelilahTab.h"         // Own interface



/* ****************************************************************************
*
* DelilahTab::DelilahTab - 
*/
DelilahTab::DelilahTab(const char* name, QWidget *parent) : QWidget(parent)
{
	QLabel*      nameLabel   = new QLabel(tr(name));
	QVBoxLayout* mainLayout  = new QVBoxLayout;

	mainLayout->addWidget(nameLabel);
	mainLayout->addStretch(1);
	setLayout(mainLayout);
}
