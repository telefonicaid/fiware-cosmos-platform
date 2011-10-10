#include <QtGui>
#include <sstream>

// Include header files for application components.
// ...

#include "ui_MainWindow.h"



class MainWindow : public QMainWindow , public Ui_MainWindow
{

  Q_OBJECT
    
      public:
  
       MainWindow(QWidget *parent = 0);

       private slots:

       void open_samson_query();

    
};

