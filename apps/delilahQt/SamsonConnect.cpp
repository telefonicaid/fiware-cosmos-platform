#include <QtGui>


// Include header files for application components.
// ...


#include "au/string.h"          // au::str()

#include "SamsonConnect.h"
#include "MainWindow.h"

#include "main_DelilahQT.h"     // Top level commands for delilahQT

extern QApplication *app;
extern MainWindow *mainWindow;      
extern SamsonConnect *samsonConnect; 


SamsonConnect::SamsonConnect( QWidget *parent ) : QWidget(parent)
{
  //Setup the 
  setupUi(this);

  // No content for the error message
  setError("");

  host->setPlaceholderText("Enter url of SAMSON controller...");

  // Connect connect Button
  connect(connectButton, SIGNAL(clicked()), this, SLOT(samson_connect()));

  // Connect cancel button with quit app
  QObject::connect(cancelButton, SIGNAL(clicked()),app, SLOT(quit()));
}


void SamsonConnect::setError( std::string error_message )
{
  errorLabel->setText( error_message.c_str() );
}

void SamsonConnect::samson_connect()
{
    // Trying to connect with something..
    std::string controller = host->text().toStdString();
    
    setError( au::str("Connecting to... %s " , controller.c_str() ) );
    
    ::connect( controller );

}
