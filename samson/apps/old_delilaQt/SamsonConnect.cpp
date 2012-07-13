#include <QtGui>


// Include header files for application components.
// ...


#include "au/string.h"          // au::str()

#include "SamsonConnect.h"
#include "MainWindow.h"

#include "DelilahConnection.h"     // DelilahConnection

extern QApplication *app;
extern MainWindow *mainWindow;      
extern SamsonConnect *samsonConnect; 

extern DelilahConnection* delilahConnection; 

SamsonConnect::SamsonConnect( QWidget *parent ) : QWidget(parent)
{
    //Setup the 
    setupUi(this);
    
    // No content for the error message
    setError("");
    
    // Connect connect Button
    connect(connectButton, SIGNAL(clicked()), this, SLOT(samson_connect()));
    
    // Connect cancel button with quit app
    QObject::connect(cancelButton, SIGNAL(clicked()),app, SLOT(quit()));
    
    // Connect localhost button
    QObject::connect(localhostButton, SIGNAL(clicked()), this, SLOT( samson_connect_localhost() ) );
    
}


void SamsonConnect::setError( std::string error_message )
{
    errorLabel->setText( error_message.c_str() );
}

void SamsonConnect::SetConnectionMessage( std::string message )
{
    errorLabel->setText( message.c_str() );
    
    // Dissable connection button
    connectButton->setEnabled ( false );
    localhostButton->setEnabled ( false );
    
}

void SamsonConnect::samson_connect_localhost()
{
    // Trying to connect with something..
    std::string controller = "localhost";

	// Show localhost on the connection box
	host->setText( "localhost" );

    SetConnectionMessage( au::str("Connecting to... %s " , controller.c_str() ) );
    delilahConnection->connect( controller );
}

void SamsonConnect::samson_connect()
{
    // Trying to connect with something..
    std::string controller = host->text().toStdString();
    SetConnectionMessage( au::str("Connecting to... %s " , controller.c_str() ) );
    delilahConnection->connect( controller );
}
