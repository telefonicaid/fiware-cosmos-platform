/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */


#include <stdio.h>      // printf
#include <stdlib.h>     // exit
#include <string.h>     // memcpy
#include <iostream>     // std::cout

#include "parseArgs/parseArgs.h"
#include "logMsg/logMsg.h"

#include "au/containers/map.h"
#include "au/RRT.h"             // au::ValueCollection 
#include "au/mutex/Token.h"
#include "au/string.h"              // au::str()
#include "au/mutex/TokenTaker.h"             

#include "samson/client/SamsonClient.h"         // samson::SamsonClient

#include "common.h"     // Common defintion

#include "MainWindow.h" // Own interface

extern samson::SamsonClient samson_client;

extern char title[1024];
extern char concept[1024];

extern std::string web_content;
extern au::Token token_web_content;

extern std::vector<Topic> topics;

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
    
    // Model for the list
    tableView->setModel( &model );
    
    
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
    
    
    // Update model for the list
    
    model.clear();

    model.setColumnCount(2);
    model.setRowCount( topics.size() );
    
    model.setHorizontalHeaderItem( 0 , new QStandardItem( QString(  "Concept" ) ) );
    model.setHorizontalHeaderItem( 1 , new QStandardItem( QString(  "Hits" ) ) );
    model.setHorizontalHeaderItem( 2 , new QStandardItem( QString(  "Hits" ) ) );
    
    
    for (size_t i = 0 ; i < topics.size() ; i++ )
    {
        //model.setItem(i, 0, new QStandardItem( QString( au::str("%s",topics[i].concept.c_str() ).c_str() ) ) );
        //model.setItem(i, 1, new QStandardItem( QString( au::str("%s", au::str(topics[i].num).c_str() ).c_str() ) ) );

        model.setData( model.index(i,0) , au::str("%s",topics[i].concept.c_str() ).c_str() );
        model.setData( model.index(i,1) , au::str("%s", au::str(topics[i].num).c_str() ).c_str() );
        model.setData( model.index(i,2) , au::str("%lu", topics[i].num ).c_str() );
    }
    
    // ------------------------------------------------------------------    
    

	tableView->resizeColumnsToContents();
    
    
}

