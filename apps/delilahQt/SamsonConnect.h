#include <QtGui>
#include <iostream>

// Include header files for application components.
// ...

#include "ui_SamsonConnect.h"
#include <assert.h>

class SamsonConnect : public QWidget , public Ui_SamsonConnect
{
  Q_OBJECT
    
      public:
  
       SamsonConnect(QWidget *parent = 0);

       // Set a particular error during connection...
       void setError( std::string error_message);

      private slots:

       void samson_connect();


 
       
};

