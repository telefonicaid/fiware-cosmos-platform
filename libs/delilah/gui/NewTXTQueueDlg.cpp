#include "NewTXTQueueDlg.h"

NewTXTQueueDlg::NewTXTQueueDlg(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);
	connect(ui.nameLineEdit, SIGNAL(textChanged(QString)), this, SIGNAL(nameChanged(QString)));
}

NewTXTQueueDlg::~NewTXTQueueDlg()
{

}

//void NewTXTQueueDlg::accept()
//{
//}
