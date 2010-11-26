/*
 * FILE:		ConfirmationDlg.cpp
 *
 * AUTHOR:		Anna Wojdel
 *
 * DESCRIPTION:
 *
 */

#include "ConfirmationDlg.h"

ConfirmationDlg::ConfirmationDlg(QWidget* parent)
    : QDialog(parent)
{
	ui.setupUi(this);
}

ConfirmationDlg::ConfirmationDlg(const QString &text, QWidget* parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	ui.textLabel->setText(text);
}

ConfirmationDlg::~ConfirmationDlg()
{

}

void ConfirmationDlg::setText(const QString &text)
{
	ui.textLabel->setText(text);
}
