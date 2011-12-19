

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

extern char title[1024];
extern char x_title[1024];
extern char y_title[1024];

extern char concept[1024];

extern au::map<std::string , au::ContinuousValueCollection<double> > value_collections;
extern au::Token token_value_collections;

extern char title[1024];
extern char concept[1024];

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

    // Create the plot
	plot = new Plot( groupBox_plot , x_title , y_title );
    
}

void MainWindow::updateTimer()
{
    au::TokenTaker tt(&token_value_collections);

    // Take a sample for all of them
    au::map<std::string , au::ContinuousValueCollection<double> >::iterator it_value_collections;
    for ( it_value_collections = value_collections.begin() ; it_value_collections != value_collections.end() ; it_value_collections++ )
        it_value_collections->second->takeSample();
    
    
    // Update the model for the plot....
    
    std::vector<std::string> labels;
    plot->clear();
    int row = 0;
    
    for ( it_value_collections = value_collections.begin() ; it_value_collections != value_collections.end() ; it_value_collections++ )
    {
        au::ContinuousValueCollection<double>* vc = it_value_collections->second;
        
        for ( int i = 0 ; i < vc->getNumSamples() ; i++)
        {
            plot->set( i , row , vc->getSample(i) );
            //plot->set( i , row , rand()%10 );
        }
        
        labels.push_back( it_value_collections->first );
        row++;
    }
    
    plot->set_legend( "Levels..", labels );
    
}

