#ifndef CONFIRMATIONDLG_H
#define CONFIRMATIONDLG_H

#include <QtGui/QDialog>
#include "ui_ConfirmationDlg.h"

class ConfirmationDlg : public QDialog
{
    Q_OBJECT

public:
    ConfirmationDlg(QWidget* parent=0);
    ConfirmationDlg(const QString &text, QWidget* parent=0);
    ~ConfirmationDlg();

    void setText(const QString &text);

private:
    Ui::ConfirmationDlgClass ui;
};

#endif // CONFIRMATIONDLG_H
