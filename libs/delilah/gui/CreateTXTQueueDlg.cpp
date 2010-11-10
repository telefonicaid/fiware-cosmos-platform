#include <iostream>

#include "CreateTXTQueueDlg.h"
#include "DelilahQtApp.h"

CreateTXTQueueDlg::CreateTXTQueueDlg(QWidget* parent)
		: QDialog(parent)
{
	ui.setupUi(this);

	show_error = false;
	connect(ui.nameLineEdit, SIGNAL(textChanged(QString)), this, SLOT(cancelError()));
}

//bool CreateTXTQueueDlg::nameValid(const QString &name)
//{
//	// TODO: Validate that name does not already exists
//	// (compare with names in existing_names list)
//
//	QString n;
//	foreach(n, existing_names)
//	{
//		if(n.startsWith(name) )
//			return false;
//	}
//
//	return true;
//}
//
//void CreateTXTQueueDlg::setName(QString _name)
//{
//	// TODO: fix me - it's only temporal to test
//	if (!nameValid(_name))
//	{
//		ui.errorPixmapLabel->setPixmap(QPixmap(QString::fromUtf8(":/icons/new_process.png")));
//		ui.errorLabel->setText("Such name already exists.");
//		name = "";
//	}
//	else
//	{
//		ui.errorPixmapLabel->setPixmap(QPixmap());
//		ui.errorLabel->setText("");
//		name = _name;
//	}
//}

void CreateTXTQueueDlg::accept()
{
	DelilahQtApp* app = (DelilahQtApp*)qApp;

	QString validation = app->validateNewQueueName(ui.nameLineEdit->text());

	if ( validation.isNull() )
		QDialog::accept();
	else
	{
		// TODOD: show some message????
		std::cout << "Name is not set correctly\n";
		ui.errorPixmapLabel->setPixmap(QPixmap(QString::fromUtf8(":/icons/new_process.png")));
		ui.errorLabel->setText(validation);
		show_error = true;
	}
}

void CreateTXTQueueDlg::cancelError()
{
	if (show_error)
	{
		ui.errorPixmapLabel->setPixmap(QPixmap());
		ui.errorLabel->setText("");
		show_error = false;
	}
}
