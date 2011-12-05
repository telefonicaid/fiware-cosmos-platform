
#include <QtGui>

#include "ui_MainWindow.h"              // Parent class

#include "Plot.h"                       // Basic plot using KDChart library

class MainWindow : public QMainWindow , public Ui_MainWindow
{    
    Q_OBJECT
    
    Plot* plot;  // Main plot using KVChart
	
public:
    
    MainWindow(QWidget *parent = 0);

private slots:
    
    void updateTimer();
    
};


