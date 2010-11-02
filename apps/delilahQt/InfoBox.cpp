#include "InfoBox.h"

InfoBox::InfoBox(QWidget *parent)
    : QDialog(parent, Qt::CustomizeWindowHint)
{
	ui.setupUi(this);

}

InfoBox::~InfoBox()
{

}

void InfoBox::setText(const QString &text)
{
	ui.textEdit->insertPlainText(text);
}
