/*
 * FILE:		CreateDataQueueDlg.cpp
 *
 * AUTHOR:		Anna Wojdel
 *
 * DESCRIPTION:
 *
 */

#include <iostream>

#include "CreateDataQueueDlg.h"
#include "DelilahQtApp.h"

CreateDataQueueDlg::CreateDataQueueDlg(QWidget* parent)
		: QDialog(parent)
{
	ui.setupUi(this);

	error_visible = false;
	connect(ui.nameLineEdit, SIGNAL(textChanged(QString)), this, SLOT(cancelError()));
}

void CreateDataQueueDlg::accept()
{
	DelilahQtApp* app = (DelilahQtApp*)qApp;

	QString validation;
	if(ui.nameLineEdit->text().isEmpty())
		validation = "Queue name is empty.";
	else
		validation = app->validateNewQueueName(ui.nameLineEdit->text());

	if ( validation.isNull() )
		QDialog::accept();
	else
		showError(validation);
}

void CreateDataQueueDlg::showError(QString error)
{
	ui.errorPixmapLabel->setPixmap(QPixmap(QString::fromUtf8(":/icons/error.png")));
	ui.errorLabel->setText(error);
	error_visible = true;
}

void CreateDataQueueDlg::cancelError()
{
	if (error_visible)
	{
		ui.errorPixmapLabel->setPixmap(QPixmap());
		ui.errorLabel->setText("");
		error_visible = false;
	}
}
