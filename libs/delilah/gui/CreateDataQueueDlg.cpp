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

	show_error = false;
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
	{
		// TODO: Replace with correct message and icon
		std::cout << "Name is not set correctly\n";
		ui.errorPixmapLabel->setPixmap(QPixmap(QString::fromUtf8(":/icons/new_process.png")));
		ui.errorLabel->setText(validation);
		show_error = true;
	}
}

void CreateDataQueueDlg::cancelError()
{
	if (show_error)
	{
		ui.errorPixmapLabel->setPixmap(QPixmap());
		ui.errorLabel->setText("");
		show_error = false;
	}
}
