#ifndef INFOBOX_H
#define INFOBOX_H

#include <QtGui/QDialog>
#include "ui_InfoBox.h"

class InfoBox : public QDialog
{
    Q_OBJECT

public:
    InfoBox(QWidget *parent = 0);
    ~InfoBox();

    void setText(const QString &text);
private:
    Ui::InfoBoxClass ui;
};

#endif // INFOBOX_H
