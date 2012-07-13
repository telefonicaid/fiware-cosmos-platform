
#include <QtGui>

#include "ui_MainWindow.h"              // Parent class


class MainWindow : public QMainWindow , public Ui_MainWindow
{    
    Q_OBJECT
    
        // Model for the list
    QStandardItemModel model;
	
public:
    
    MainWindow(QWidget *parent = 0);

private slots:
    
    void updateTimer();
    
};


