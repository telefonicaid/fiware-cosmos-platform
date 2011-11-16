

#include <stdio.h>      // printf
#include <stdlib.h>     // exit
#include <string.h>     // memcpy
#include <iostream>     // std::cout

#include "parseArgs/parseArgs.h"
#include "logMsg/logMsg.h"

#include "au/map.h"
#include "au/RRT.h"             // au::ValueCollection 
#include "au/Token.h"
#include "au/string.h"              // au::str()
#include "au/TokenTaker.h"             

#include "samson/client/SamsonClient.h"         // samson::SamsonClient

#include "common.h"     // Common defintion

#include "MainWindow.h" // Own interface

extern samson::SamsonClient samson_client;


extern au::map<std::string , au::ContinuousValueCollection<double> > value_collections;
extern au::Token token_value_collections;

extern char title[1024];
extern char concept[1024];

extern std::string web_content;
extern au::Token token_web_content;


MainWindow::MainWindow( QWidget *parent ) : QMainWindow(parent)
{
    
    //Setup the 
    setupUi(this);
    
    // Connections
    // connect(samsonQueryPushButton, SIGNAL(clicked()), this, SLOT(open_samson_query()));
    //connect( treeUpdateButton , SIGNAL(clicked()) , this , SLOT( updateTree() ) );

	// Set the top label with information about the connected queue
    if( strcmp( title , "no_title") == 0)
        label_info->setText( au::str("Monitoring %s" , concept ).c_str() );
    else
        label_info->setText( title );
    
    // General update model
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTimer()));
    timer->start(1000);

    
}



void MainWindow::updateTimer()
{
    // Take contentof web_content
    au::TokenTaker tt(&token_web_content);
    webView->setHtml( web_content.c_str() );
}

