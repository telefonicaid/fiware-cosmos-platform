#include <QtGui>


// Include header files for application components.
// ...


#include "MainWindow.h"
#include "SamsonQueryWidget.h"

extern SamsonQueryWidget* samsonQueryWidget;

MainWindow::MainWindow( QWidget *parent ) : QMainWindow(parent)
{
    //Setup the 
    setupUi(this);
    
    // Connections
    connect(samsonQueryPushButton, SIGNAL(clicked()), this, SLOT(open_samson_query()));
    
}


void MainWindow::open_samson_query()
{
    samsonQueryWidget->show();
    
}
